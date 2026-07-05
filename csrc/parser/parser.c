/**
 * @file parser.c
 * @author Moritz Heinzmann, Paul Gramescu
 * @date 06/18/2026
 *
 * @note The parser is relatively silly and only checks whether all tags are
 * present and whether everything was entered in the correct order. The valid
 * value range of the parsed setting is checked in the task, which then limits
 * the values to the maximum value when writing them to the queue for the task
 * that manages the DAC, should the value be exceeded.
*/

/*******************************************************************************
*                               INCLUDES
*******************************************************************************/

#include "parser.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/*******************************************************************************
*                               DEFINES
*******************************************************************************/



/*******************************************************************************
*                            STATIC PROTOTYPES
*******************************************************************************/

static index_t get_tags(char* buffer);

/*******************************************************************************
*                           GLOBAL FUNCTIONS
*******************************************************************************/

static index_t get_tags(char* buffer)
{
    index_t tag = {
        .hash = 0,
        .dot = 0,
        .c = 0,
        .w = 0,
        .f = 0,
        .p = 0,
		.a = 0,
		.o = 0,
	.c_size = 0,
	.w_size = 0,
	.f_size = 0,
	.a_size = 0,
	.p_size = 0,
	.o_size = 0,
    };

    /* Search for indices of tags */
    size_t len = strlen(buffer);
    for (size_t i = 0; i < len; i++)
    {
        if (buffer[i] == '#')
        {
                tag.hash = i;
        }
        else if (buffer[i] == 'c')
        {
                tag.c = i;
        }
        else if (buffer[i] == 'w')
        {
                tag.w = i;
        }
        else if (buffer[i] == 'f')
        {
                tag.f = i;
        }
        else if (buffer[i] == 'p')
        {
                tag.p = i;
        }
        else if (buffer[i] == 'a')
        {
                tag.a = i;
        }
        else if (buffer[i] == 'o')
        {
                tag.o = i;
        }
        else if (buffer[i] == '.')
        {
                tag.dot = i;
        }

    }

    /* Check that all tags are present */
    if ( tag.dot == 0 || tag.c == 0 || tag.w == 0 ||
         tag.f == 0   || tag.p == 0 || tag.a == 0 ||
		 tag.o == 0)
    {
        tag.hash = -1;
        return tag;
    }

    tag.c_size = tag.w - tag.c;
    tag.w_size = tag.f - tag.w;
    tag.f_size = tag.p - tag.f;
    tag.p_size = tag.a - tag.p;
    tag.a_size = tag.o - tag.a;
    tag.o_size = tag.dot - tag.o;

    /* Check that all tags have been entered in the correct order */
    if ( tag.f_size < 0 || tag.p_size < 0 ||
         tag.c_size < 0 || tag.w_size < 0 ||
		 tag.a_size < 0 || tag.o_size < 0 )
    {
        tag.hash = -1;
        return tag;
    }

    return tag;
}

/*
 * This function will take the input string and parse it.
 * @returns waveform settings as waveform_t
 * */
waveform_t parse_settings(char* buffer)
{
    waveform_t user_settings = {
            .channel = 0,
    		.type = 0,
    		.frequency = 0,
    		.phase = 0,
			.amplitude_percentage = 0,
			.offset = 0,
            };
    char channel[PARSER_KEYS_BUFFER_SIZE];
    char waveform[PARSER_KEYS_BUFFER_SIZE];
    char frequency[PARSER_KEYS_BUFFER_SIZE];
    char phase[PARSER_KEYS_BUFFER_SIZE];
    char amplitude_percentage[PARSER_KEYS_BUFFER_SIZE];
    char offset[PARSER_KEYS_BUFFER_SIZE];

    const index_t tag = get_tags(buffer);
    /* Return error code! tag.hash == -1 indicates that at least one tag
     * is missing or have not been entered in the correct order */
    if (tag.hash == -1)
    {
        user_settings.frequency = -1;

        return user_settings;
    }

    /* Extract substrings */
    strncpy(&channel[0], buffer + tag.c + 1, tag.c_size - 1);
    channel[tag.c_size - 1] = '\0';
    strncpy(&waveform[0], buffer + tag.w + 1, tag.w_size - 1);
    waveform[tag.w_size - 1] = '\0';
    strncpy(&frequency[0], buffer + tag.f + 1, tag.f_size - 1);
    frequency[tag.f_size - 1] = '\0';
    strncpy(&phase[0], buffer + tag.p + 1, tag.p_size - 1);
    phase[tag.p_size - 1] = '\0';
    strncpy(&amplitude_percentage[0], buffer + tag.a + 1, tag.a_size - 1);
    amplitude_percentage[tag.a_size - 1] = '\0';
    strncpy(&offset[0], buffer + tag.o + 1, tag.o_size - 1);
    offset[tag.o_size - 1] = '\0';

    /* Write parsed setting into struct.
     *
     * Ignore all characters except the first one in waveform and channel, since
     * these settings have a maximum length of one character. */
    if (waveform[0] == 'R')
        user_settings.type = RECTANGLE;
    else if (waveform[0] == 'T')
        user_settings.type = TRIANGLE;
    else if (waveform[0] == 'S')
        user_settings.type = SAW;
    else if (waveform[0] == 'I')
        user_settings.type = SINE;
    /* Converting string by subtracting ASCII 0 */
    user_settings.channel = channel[0] - '0';
    /* float typecast, since atof returns double */
    user_settings.frequency = (float) atof(frequency);
    user_settings.phase = (float) atof(phase);
    user_settings.amplitude_percentage = atoi(amplitude_percentage);
    user_settings.offset = atoi(offset);

    return user_settings;
}

/*******************************************************************************
*                     INTERRUPT SERVICE ROUTINES
*******************************************************************************/



/*================================= EOF ======================================*/
