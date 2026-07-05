/**
 * @file task_wave.c
 * @author Moritz Heinzmann
 * @date 06/02/2026
 *
 * FreeRTOS-Task for outputting signals via the DAC (PmodDA2 Board).
 *
 * The hardware timer ISR sets the frequency via the semaphore.
 * The waveform parameters can be modified at run time via a queue.
*/

/*******************************************************************************
*                               INCLUDES
*******************************************************************************/

#include "task_wave.h"

#include <stdint.h>

#include <xparameters.h>
#include <xil_printf.h>
#include "../waveforms/waveforms.h"

/*******************************************************************************
*                               DEFINES
*******************************************************************************/



/*******************************************************************************
*                               TYPEDEFS
*******************************************************************************/



/*******************************************************************************
*                            GLOBAL VARIABLES
*******************************************************************************/

TaskHandle_t xtask_wave;
QueueHandle_t wave_queue;

/*******************************************************************************
*                            STATIC VARIABLES
*******************************************************************************/

static ETwinSPI spi;

static waveform_t wave_ch1;
static waveform_t wave_ch2;

static phase_acc_t acc_ch1;
static phase_acc_t acc_ch2;

/*******************************************************************************
*                           STATIC FUNCTIONS
*******************************************************************************/

static void to_big_endian(uint8_t* buffer, uint16_t value)
{
	buffer[0] = (value >> 8);
	buffer[1] = (value & 0xFF);
}

/*******************************************************************************
*                           GLOBAL FUNCTIONS
*******************************************************************************/

BaseType_t task_wave_init(void)
{
    ETwinSPI_Initialize(&spi, XPAR_EES_TWIN_SPI_0_S00_AXI_BASEADDR, 32, 0, 1);

    /* Init CH1 */
    wave_ch1.channel = CH1;
    wave_ch1.type = SINE;
    wave_ch1.frequency = 100.0f;
    wave_ch1.phase = 0.0f;
    wave_ch1.amplitude_percentage = 100;
    wave_ch1.offset = WAVEFORM_DAC_MID;

    /* Init CH2 */
    wave_ch2.channel = CH2;
    wave_ch2.type = TRIANGLE;
    wave_ch2.frequency = 100.0f;
    wave_ch2.phase = 0.0f;
    wave_ch2.amplitude_percentage = 100;
    wave_ch2.offset = WAVEFORM_DAC_MID;

	waveforms_init_channel(&acc_ch1,
			       wave_ch1.frequency,
			       wave_ch1.phase,
				   wave_ch1.amplitude_percentage,
				   wave_ch1.offset);

	waveforms_init_channel(&acc_ch2,
			       wave_ch2.frequency,
			       wave_ch2.phase,
				   wave_ch2.amplitude_percentage,
				   wave_ch2.offset);

    /* Create queue for communication with the UART task */
    if ((wave_queue = xQueueCreate(TASK_WAVE_QUEUE_LEN,
    		                      sizeof(waveform_t))) == NULL)
    {
    	return pdFAIL;
    }

    return pdPASS;
}

void task_wave(void* pvParameters)
{

    uint8_t  buffer_ch1[2];
    uint8_t  buffer_ch2[2];
    uint16_t dac_ch1;
    uint16_t dac_ch2;

    waveform_t new_wave;

    while(1)
    {
	/* Wait for timer ISR (block task) */
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

	/* Take new waveform config from queue; 0 -> non blocking */
	while(xQueueReceive(wave_queue, &new_wave, 0) == pdPASS)
	{
		acc_ch1.phase_acc = 0;
		acc_ch2.phase_acc = 0;

	    if (new_wave.channel == CH1)
	    {
		wave_ch1 = new_wave;
		waveforms_init_channel(&acc_ch1,
				       wave_ch1.frequency,
				       wave_ch1.phase,
					   wave_ch1.amplitude_percentage,
					   wave_ch1.offset);
	    }
	    else if (new_wave.channel == CH2)
	    {
		wave_ch2 = new_wave;
		waveforms_init_channel(&acc_ch2,
				       wave_ch2.frequency,
				       wave_ch2.phase,
					   wave_ch2.amplitude_percentage,
					   wave_ch2.offset);
	    }
	}

	/* Calc new DAC values and output them */
	dac_ch1 = waveforms_tick(&acc_ch1, wave_ch1.type);
	dac_ch2 = waveforms_tick(&acc_ch2, wave_ch2.type);

	to_big_endian(buffer_ch1, dac_ch1);
	to_big_endian(buffer_ch2, dac_ch2);

	ETwinSPI_WriteOnly(&spi, ETS_CH0 | ETS_CH1, 2, buffer_ch1, buffer_ch2);
    }
}
void behind_task_wave(void) {}

/*================================= EOF ======================================*/
