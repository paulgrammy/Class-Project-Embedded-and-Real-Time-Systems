#ifndef UNICODE_SUBS_H
#define UNICODE_SUBS_H

static struct {
    char ASCII;
    char UTF8[7];
} unicode_subs[] =
{
    // - use ASCII characters you don't need or 
    //   or 7-bit hex values <= '\x7f'
    // - extend with whatever \u 4-hex digit or \U 8-hex
    //   digit unicode substitutions you'd like
    { '\x01', "\U0001f609"}, // 😉 winking smiley 

    { '$', "\u2588"}, // █ light block 
    { '~', "\u00b7"}, // · center dot
    { '%', "\u2022"}  // • big dot 
};


static const size_t uc_subs_elements
    = sizeof(unicode_subs) / sizeof(unicode_subs[0]); 

#endif // UNICODE_SUBS_H
