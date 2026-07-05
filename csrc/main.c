/**
 * @file main.c
 * @author Moritz Heinzmann, Paul Gramescu
 * @date 06/02/2026
*/

/*******************************************************************************
*                               INCLUDES
*******************************************************************************/

#include "FreeRTOS.h"
#include "task.h"
#include <xparameters.h>
#include "xil_printf.h"
#include "xtmrctr.h"
#include "xil_exception.h"

#include "tasks/task_wave.h"
#include "tasks/task_uart.h"
#include "waveforms/waveforms.h"

#include "ees_profiler/ees_profiler_v1_7.h"

/*******************************************************************************
*                               DEFINES
*******************************************************************************/

#define NUM_TASK 2

// macros for timing
#define CYCLES_PER_SECOND (XPAR_CPU_M_AXI_DP_FREQ_HZ)
#define TIMER_PERIOD (CYCLES_PER_SECOND / WAVEFORM_F_SAMPLE)
#define TIMER_B_CHANNEL 1
#define SUBSAMPLE_RATE 10

/*******************************************************************************
*                               TYPEDEFS
*******************************************************************************/



/*******************************************************************************
*                            GLOBAL VARIABLES
*******************************************************************************/

extern TaskHandle_t pxCurrentTCB;

/*******************************************************************************
*                            STATIC VARIABLES
*******************************************************************************/

static XTmrCtr TimerB;

/*******************************************************************************
*                            GLOBAL PROTOTYPES
*******************************************************************************/

void hardware_timer_init(void);
void timer_isr_handler(void);

/*******************************************************************************
*                                 MACROS
*******************************************************************************/



/*******************************************************************************
* -------------------------------- MAIN --------------------------------------*/

int main(void)
{
	EProfiler HW_Profiler;
	EProfiler_Initialize(&HW_Profiler, XPAR_EES_PROFILER_0_S00_AXI_BASEADDR);
	EProfiler_SetSubsampleStretchRate(&HW_Profiler, SUBSAMPLE_RATE);

    BaseType_t create_status[NUM_TASK];
    BaseType_t init_status[NUM_TASK];

    init_status[0] = task_wave_init();
    init_status[1] = task_uart_init();

    create_status[0] = xTaskCreate(task_wave, "OUTPUT", 2048,
                                 NULL, 5, &xtask_wave);
    create_status[1] = xTaskCreate(task_uart, "UART", 4096,
                                 NULL, 3, &xtask_uart);

    if (!create_status[0] || !init_status[0] || !create_status[1] || !init_status[1])
    {
        xil_printf("ERROR: FreeRTOS resource creation fail.\n");
        return 1;
    }

    EProfiler_ConfigureCounter(&HW_Profiler, EPR_CH_A, EPR_MEM_VAL, U32(&pxCurrentTCB), U32(xtask_wave));
    EProfiler_ConfigureCounter(&HW_Profiler, EPR_CH_B, EPR_MEM_VAL, U32(&pxCurrentTCB), U32(xtask_uart));

    /* Start OS -> hand over control to the scheduler */
    xil_printf("START: FreeRTOS scheduler.\nSend -help- for how-to use.");
    hardware_timer_init();
    vTaskStartScheduler();

    return 0;
}

/*******************************************************************************
*                           GLOBAL FUNCTIONS
*******************************************************************************/

void hardware_timer_init(void)
{
	// Initializing driver instance structs
	XTmrCtr_Initialize( &TimerB, XPAR_AXI_TIMER_B_DEVICE_ID);

	// Activate processor exceptions
	Xil_ExceptionInit();
	Xil_ExceptionEnable();

	// Connect ISR and activate timer interrupt
	xPortInstallInterruptHandler(XPAR_AXI_INTC_0_AXI_TIMER_B_INTERRUPT_INTR,
			                         (XInterruptHandler)timer_isr_handler,
									 (void*) &TimerB);
	vPortEnableInterrupt(XPAR_AXI_INTC_0_AXI_TIMER_B_INTERRUPT_INTR);

	// Configure timer for interrupt, auto-reload (cyclic), counting down
	XTmrCtr_SetOptions( &TimerB, TIMER_B_CHANNEL, XTC_INT_MODE_OPTION |
						                 XTC_AUTO_RELOAD_OPTION |
										 XTC_DOWN_COUNT_OPTION);
	XTmrCtr_SetResetValue( &TimerB, TIMER_B_CHANNEL, TIMER_PERIOD);
	XTmrCtr_Start( &TimerB, TIMER_B_CHANNEL);
}

/*******************************************************************************
*                     INTERRUPT SERVICE ROUTINES
*******************************************************************************/

void timer_isr_handler(void)
{
    /* Wake the "wave" task via timer ISR */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(xtask_wave, &xHigherPriorityTaskWoken);

    /* Clear interrupt flag */
    volatile uint32_t *TCSR1_ptr = (uint32_t*)(TimerB.BaseAddress + 0x10);
    *TCSR1_ptr = *TCSR1_ptr;

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*================================= EOF ======================================*/
