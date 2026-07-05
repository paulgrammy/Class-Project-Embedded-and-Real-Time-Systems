/**
 * @file task_uart.c
 * @author Moritz Heinzmann, Paul Gramescu
 * @date MM/DD/YYYY
*/

/*******************************************************************************
*                               INCLUDES
*******************************************************************************/

#include "task_uart.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <xuartlite.h>
#include <xuartlite_l.h>
#include "xintc.h"
#include "xparameters.h"

#include "../waveforms/waveforms.h" // TODO: change to waveforms.h
#include "../parser/parser.h"
#include "../tasks/task_wave.h"

/*******************************************************************************
*                               DEFINES
*******************************************************************************/

#define MAX_SETTINGS_SIZE 20+1 //+1 for 0-termination
#define MIN_SETTINGS_SIZE 14+1 //+1 for 0-termination
#define UART_QUEUE_SIZE 64

#define START_CHAR '#'
#define STOP_CHAR  '.'

/*******************************************************************************
*                               TYPEDEFS
*******************************************************************************/

typedef enum
{
    WAITING = 0,
    RECEIVING,
    DISCARDING,
}rx_state_t;

/*******************************************************************************
*                            GLOBAL VARIABLES
*******************************************************************************/

TaskHandle_t xtask_uart;
QueueHandle_t uart_queue;

/*******************************************************************************
*                            STATIC VARIABLES
*******************************************************************************/

static XUartLite uart_device;

/*******************************************************************************
*                            STATIC PROTOTYPES
*******************************************************************************/

static void ISR_UART(void);
static char *usage = "\n\nEES WAVEFORM GENERATOR USAGE\n\n"
		             "Format: #c<>w<>f<>p<>a<>o<>.\n\n"
                     "Channel    c:   <1|2>\n"
		             "Waveform   w:   <R>ectangle | <S>aw | <T>riangle | <I>Sine\n"
                     "Frequency  f:   <1-5000>\n"
                     "Phase      p:   <0-359>\n"
 	 	 	 	 	 "Amplitude  a:   <0-100>%\n"
				 	 "Offset 	 o:	  <0-4095>\n";

/*******************************************************************************
*                            GLOBAL PROTOTYPES
*******************************************************************************/



/*******************************************************************************
*                                 MACROS
*******************************************************************************/



/*******************************************************************************
*                           GLOBAL FUNCTIONS
*******************************************************************************/

BaseType_t task_uart_init()
{
    if ((uart_queue = xQueueCreate(UART_QUEUE_SIZE, sizeof(char))) == NULL)
    	return pdFAIL;

    XUartLite_Initialize(&uart_device, XPAR_AXI_UARTLITE_0_DEVICE_ID);

    // Connect Interrupt Controller with FAST IRQ for Uartlite, activate
    xPortInstallInterruptHandler(XPAR_AXI_INTC_0_AXI_UARTLITE_0_INTERRUPT_INTR,
		                 (XInterruptHandler)ISR_UART,
				 (void*) &uart_device);
    vPortEnableInterrupt(XPAR_AXI_INTC_0_AXI_UARTLITE_0_INTERRUPT_INTR);

    XUartLite_EnableInterrupt(&uart_device);

    return pdPASS;
}

void task_uart(void* pvParameters)
{
    volatile rx_state_t state = WAITING;
    char rx_char;
    char received_string[MAX_SETTINGS_SIZE] = {'\0'};
    volatile uint8_t index = 0;
    waveform_t settings = {
        .channel = 0,
		.type = 0,
		.frequency = 0,
		.phase = 0,
		.amplitude_percentage = 0,
        };

    while(1)
    {
		if (xQueueReceive(uart_queue, &rx_char, portMAX_DELAY) == pdPASS)
		{
			switch(state)
			{
			case WAITING:
				if (rx_char == START_CHAR || rx_char == 'h')
				{
				memset(received_string, 0, sizeof(received_string));
				index = 0;
				received_string[index++] = rx_char;
				state = RECEIVING;
				}
				break;

			case RECEIVING:
				if (rx_char == STOP_CHAR)
				{
					/* Reached end of the setting string. At this point only
					 * the length of the string is vaild - the content may not.
					 * The parser will raise an error if the content is invalid.
					 * The system checks whether the settings are within the
					 * valid range only after parsing.
					 */
					received_string[index++] = rx_char;
					received_string[index] = '\0';
					settings = parse_settings(received_string);

					if (settings.frequency < 0)
					{
						xil_printf("%s", usage);
						state = WAITING;
						break;
					}
					// TODO: remove magic numbers with defines from waveforms.h
					else if (settings.channel > 2 || settings.type > 3 ||
							 settings.frequency > WAVEFORM_F_MAX || settings.phase > 359)
					{
						xil_printf("%s", usage);
						state = WAITING;
						break;
					}
					/* This point is only reached when the settings
					 * are valid. */
		#if 1 // TODO: change to wave_queue
					xQueueSend(wave_queue, &settings, (TickType_t) 0);
		#endif
		#if 0
					// WARNING: ONLY FOR DEBUGING, REMOVE AFTERWARDS!
					xil_printf("%s", received_string);
		#endif
					state = WAITING;
					break;
				}
				else if (rx_char == 'p' && received_string[0] == 'h' && index == 3)
				{
					received_string[index++] = rx_char;
					received_string[index] = '\0';
					if (strcmp(received_string, "help") == 0)
						xil_printf("%s", usage);
					state = WAITING;
					break;
				}
				else if (index < MAX_SETTINGS_SIZE)
					received_string[index++] = rx_char;
				else state = DISCARDING;

				break;

			case DISCARDING:
				if (rx_char == START_CHAR)
				{
				index = 0;
				received_string[index++] = rx_char;
				state = RECEIVING;
				}
				break;

			default:
				state = WAITING;
				break;
			}
	}
    }
}
void behind_task_uart(void) {}

/*******************************************************************************
*                     INTERRUPT SERVICE ROUTINES
*******************************************************************************/

void ISR_UART(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    char received_byte;

    uint8_t STAT_REG = XUartLite_GetStatusReg(uart_device.RegBaseAddress);

    while (STAT_REG & XUL_SR_RX_FIFO_VALID_DATA) {  // IRQ reason #1: received character in RX FIFO
    	// fill RX queue
    	received_byte = XUartLite_RecvByte(uart_device.RegBaseAddress);
    	xQueueSendToBackFromISR(uart_queue, &received_byte, &xHigherPriorityTaskWoken);
    	STAT_REG = XUartLite_GetStatusReg(uart_device.RegBaseAddress); // check if there's more
    }

    // Yield to higher priority task if one was woken
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*================================= EOF ======================================*/
