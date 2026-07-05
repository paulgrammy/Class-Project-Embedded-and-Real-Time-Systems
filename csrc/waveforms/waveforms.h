/**
 * @file waveforms.h
 * @author Moritz Heinzmann
 * @date 05/20/2026
*/

#ifndef WAVEFORMS_H
#define WAVEFORMS_H

/*******************************************************************************
*                               INCLUDES
*******************************************************************************/

#include <stdint.h>
#include "../ees_twinspi/ees_twinspi_v1_0.h"

/*******************************************************************************
*                               DEFINES
*******************************************************************************/

#define WAVEFORM_DAC_BITS 12UL
#define WAVEFORM_DAC_MAX  ((1 << WAVEFORM_DAC_BITS) - 1)
#define WAVEFORM_DAC_MID  (WAVEFORM_DAC_MAX / 2)

#define WAVEFORM_F_SAMPLE 50000.0f
#define WAVEFORM_F_MIN    1.0f
#define WAVEFORM_F_MAX    5000.0f

#define WAVEFORM_AMPLITUDE_PECENTAGE_MAX 100

#define PI                3.14159265358f
#define TWO_PI            (2.0f * PI)

/*******************************************************************************
*                               TYPEDEFS
*******************************************************************************/

typedef enum
{
    RECTANGLE = 0,
    TRIANGLE,
    SAW,
	SINE,
}waveform_type_t;

typedef enum
{
    CH1 = ETS_CH0,
    CH2 = ETS_CH1,
}waveform_channel_t;

typedef struct
{
    float phase_acc;
    float phase_inc;
    float amplitude;
    float offset;
}phase_acc_t;

typedef struct
{
    waveform_channel_t channel;
    waveform_type_t type;
    float frequency;
    float phase;
    uint8_t amplitude_percentage;
    uint16_t offset;
}waveform_t;

/*******************************************************************************
*                            GLOBAL VARIABLES
*******************************************************************************/



/*******************************************************************************
*                            GLOBAL PROTOTYPES
*******************************************************************************/

void     waveforms_init_channel(phase_acc_t *acc, float freq, float phase, uint8_t amplitude, uint16_t offset);
uint16_t waveforms_tick(phase_acc_t *acc, waveform_type_t type);

/*================================= EOF ======================================*/
#endif // WAVEFORMS_H
