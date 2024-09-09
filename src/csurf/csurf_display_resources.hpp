#ifndef CSURF_DISPLAY_RESOURCES_H_
#define CSURF_DISPLAY_RESOURCES_H_

static const int HEADER_LENGTH = 5;
static int SYSEX_HEADER[HEADER_LENGTH] = {0xf0, 0x00, 0x01, 0x06, 0x02};

enum DisplayActions
{
    DISPLAY_ACTION_DISPLAY = 0x12,
    DISPLAY_ACTION_MODE = 0x13,
};

enum DisplayMode
{
    DISPLAY_MODE_1 = 0x00,
    DISPLAY_MODE_2 = 0x01,
    DISPLAY_MODE_3 = 0x02,
    DISPLAY_MODE_4 = 0x03,
    DISPLAY_MODE_5 = 0x04,
    DISPLAY_MODE_6 = 0x05,
    DISPLAY_MODE_7 = 0x06,
    DISPLAY_MODE_8 = 0x07,
    DISPLAY_MODE_9 = 0x08,
    DISPLAY_MODE_10 = 0x09,
};

enum Alignment
{
    ALIGN_CENTER = 0,
    ALIGN_LEFT = 1,
    ALIGN_RIGHT = 2,
};

enum Inverted
{
    NON_INVERT = 0,
    INVERT = 4
};

#endif
