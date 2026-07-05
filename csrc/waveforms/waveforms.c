/**
 * @file waveforms.c
 * @author Moritz Heinzmann
 * @date 05/20/2026
*/

/*******************************************************************************
*                               INCLUDES
*******************************************************************************/

#include "waveforms.h"

/*******************************************************************************
*                               DEFINES
*******************************************************************************/



/*******************************************************************************
*                               TYPEDEFS
*******************************************************************************/



/*******************************************************************************
*                           STATIC PROTOTYPES
*******************************************************************************/



/*******************************************************************************
*                            GLOBAL VARIABLES
*******************************************************************************/



/*******************************************************************************
*                            STATIC VARIABLES
*******************************************************************************/



/*******************************************************************************
*                           STATIC FUNCTIONS
*******************************************************************************/



/*******************************************************************************
*                           GLOBAL FUNCTIONS
*******************************************************************************/

void waveforms_init_channel(phase_acc_t *acc, float freq, float phase, uint8_t amplitude_percentage, uint16_t offset)
{
	if (freq > WAVEFORM_F_MAX) freq = WAVEFORM_F_MAX;
	if (freq < WAVEFORM_F_MIN) freq = WAVEFORM_F_MIN;

	float phase_rad = (phase / 360.0f) * TWO_PI;
	if (phase_rad < 0.0f)    phase_rad += TWO_PI;
	if (phase_rad >= TWO_PI) phase_rad -= TWO_PI;

	if (amplitude_percentage > WAVEFORM_AMPLITUDE_PECENTAGE_MAX)
		amplitude_percentage = WAVEFORM_AMPLITUDE_PECENTAGE_MAX;
	else if (amplitude_percentage < 0) amplitude_percentage = 0;
	acc->amplitude = ((float) amplitude_percentage / 100) * WAVEFORM_DAC_MAX;

	acc->offset = offset;
	acc->phase_acc = phase_rad;
	acc->phase_inc = TWO_PI * (freq / WAVEFORM_F_SAMPLE);
}

uint16_t waveforms_tick(phase_acc_t *acc, waveform_type_t type)
{
	float phi = acc->phase_acc;
	int16_t out = 0;

	switch (type)
	{
		case TRIANGLE:
		{
			float n;
			if (phi < PI)
				n = phi / PI;			/* 0 -> 1 */
			else
				n = 2.0f - (phi / PI);  /* 1 -> 0 */

			out =  n * acc->amplitude + acc->offset - WAVEFORM_DAC_MID;

			if (out > WAVEFORM_DAC_MAX)
				out = (uint16_t) WAVEFORM_DAC_MAX;
			else if (out < 0)
				out = (uint16_t) 0;
			else
				out = (uint16_t) out;

			break;
		}
		case RECTANGLE:
		{
			/* max value or 0 (u: unsigned) */
			out = acc->offset + WAVEFORM_DAC_MID - ( (phi < PI) ? acc->amplitude : 0u );

			if (out > WAVEFORM_DAC_MAX)
				out = (uint16_t) WAVEFORM_DAC_MAX;
			else if (out < 0)
				out = (uint16_t) 0;
			else
				out = (uint16_t) out;

			break;
		}
		case SAW:
		{
			float n = phi / TWO_PI;
			out = n * acc->amplitude + acc->offset - WAVEFORM_DAC_MID;

			if (out > WAVEFORM_DAC_MAX)
				out = (uint16_t) WAVEFORM_DAC_MAX;
			else if (out < 0)
				out = (uint16_t) 0;
			else
				out = (uint16_t) out;

			break;
		}
		case SINE:
		{
			float n;
			if(phi < TWO_PI)
			{
				n = phi / TWO_PI;
				n = 4.0f * n * (1.0f - n);
				n = n*n;
			}
			else
			{
				n = (phi - TWO_PI)/ TWO_PI;
				n = -4.0f * n * (1.0f - n);
				n = -1 * (n*n);
			}

			out = (n * acc->amplitude) + acc->offset - WAVEFORM_DAC_MID;

			if (out > WAVEFORM_DAC_MAX)
				out = (uint16_t) WAVEFORM_DAC_MAX;
			else if (out < 0)
				out = (uint16_t) 0;
			else
				out = (uint16_t) out;

			break;
		}

		default:
			out = 0;
			break;
	}

	/* Increment phase and check for overflow */
	acc->phase_acc += acc->phase_inc;
	if (acc->phase_acc >= TWO_PI)
		acc->phase_acc -= TWO_PI;

	return out;
}

/*================================= EOF ======================================*/
