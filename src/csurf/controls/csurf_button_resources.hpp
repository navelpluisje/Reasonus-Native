#ifndef CSURF_BUTTON_RESOURCES_H_
#define CSURF_BUTTON_RESOURCES_H_

// Buttons
enum Btn_Types
{
    // General Controls (Left Side)
    BTN_ARM = 0x00,
    BTN_SOLO_CLEAR = 0x01,
    BTN_MUTE_CLEAR = 0x02,
    BTN_BYPASS = 0x03,
    BTN_MACRO = 0x04,
    BTN_LINK = 0x05,
    BTN_SHIFT_LEFT = 0x46,

    // Channel Strip Controls
    BTN_SOLO_1 = 0x08,
    BTN_SOLO_2 = 0x09,
    BTN_SOLO_3 = 0x0a,
    BTN_SOLO_4 = 0x0b,
    BTN_SOLO_5 = 0x0c,
    BTN_SOLO_6 = 0x0d,
    BTN_SOLO_7 = 0x0e,
    BTN_SOLO_8 = 0x0f,

    BTN_SOLO_9 = 0x50,
    BTN_SOLO_10 = 0x51,
    BTN_SOLO_11 = 0x52,
    BTN_SOLO_12 = 0x53,
    BTN_SOLO_13 = 0x54,
    BTN_SOLO_14 = 0x55,
    BTN_SOLO_15 = 0x59,
    BTN_SOLO_16 = 0x57,

    BTN_MUTE_1 = 0x10,
    BTN_MUTE_2 = 0x11,
    BTN_MUTE_3 = 0x12,
    BTN_MUTE_4 = 0x13,
    BTN_MUTE_5 = 0x14,
    BTN_MUTE_6 = 0x15,
    BTN_MUTE_7 = 0x16,
    BTN_MUTE_8 = 0x17,

    BTN_MUTE_9 = 0x78,
    BTN_MUTE_10 = 0x79,
    BTN_MUTE_11 = 0x7a,
    BTN_MUTE_12 = 0x7b,
    BTN_MUTE_13 = 0x7c,
    BTN_MUTE_14 = 0x7d,
    BTN_MUTE_15 = 0x7e,
    BTN_MUTE_16 = 0x7f,

    BTN_SELECT_1 = 0x18,
    BTN_SELECT_2 = 0x19,
    BTN_SELECT_3 = 0x1a,
    BTN_SELECT_4 = 0x1b,
    BTN_SELECT_5 = 0x1c,
    BTN_SELECT_6 = 0x1d,
    BTN_SELECT_7 = 0x1e,
    BTN_SELECT_8 = 0x1f,

    BTN_SELECT_9 = 0x07,
    BTN_SELECT_10 = 0x21,
    BTN_SELECT_11 = 0x22,
    BTN_SELECT_12 = 0x23,
    BTN_SELECT_13 = 0x24,
    BTN_SELECT_14 = 0x25,
    BTN_SELECT_15 = 0x26,
    BTN_SELECT_16 = 0x27,

    BTN_TOUCH_1 = 0x68,
    BTN_TOUCH_2 = 0x69,
    BTN_TOUCH_3 = 0x6a,
    BTN_TOUCH_4 = 0x6b,
    BTN_TOUCH_5 = 0x6c,
    BTN_TOUCH_6 = 0x6d,
    BTN_TOUCH_7 = 0x6e,
    BTN_TOUCH_8 = 0x6f,

    // Fader Mode Buttons
    BTN_TRACK = 0x28,
    BTN_EDIT_PLUGINS = 0x2b,
    BTN_SEND = 0x29,
    BTN_PAN = 0x2a,

    // Session Navigator
    BTN_PREV = 0x2e,
    BTN_NEXT = 0x2f,
    BTN_CHANNEL = 0x36,
    BTN_ZOOM = 0x37,
    BTN_SCROLL = 0x38,
    BTN_BANK = 0x39,
    BTN_MASTER = 0x3a,
    BTN_CLICK = 0x3b,
    BTN_SECTION = 0x3c,
    BTN_MARKER = 0x3d,

    // Mix Management
    BTN_AUDIO = 0x3e,
    BTN_VI = 0x3f,
    BTN_BUS = 0x40,
    BTN_VCA = 0x41,
    BTN_ALL = 0x42,
    BTN_SHIFT_RIGHT = 0x06,

    // Automation
    BTN_READ = 0x4a,
    BTN_WRITE = 0x4b,
    BTN_TRIM = 0x4c,
    BTN_TOUCH = 0x4d,
    BTN_LATCH = 0x4e,
    BTN_OFF = 0x4f,

    // Transport
    BTN_LOOP = 0x56,
    BTN_REWIND = 0x5b,
    BTN_FORWARD = 0x5c,
    BTN_STOP = 0x5d,
    BTN_PLAY = 0x5e,
    BTN_RECORD = 0x5f,
    BTN_FOOTSWITCH = 0x4f,
};

// Button Values
enum Btn_Value
{
    BTN_VALUE_OFF = 0x00,
    BTN_VALUE_ON = 0x7f,
    BTN_VALUE_BLINK = 0x01,
};

#define MIDI_MESSAGE_BUTTON 0x90
#define MIDI_MESSAGE_COLOR_RED 0x91
#define MIDI_MESSAGE_COLOR_GREEN 0x92
#define MIDI_MESSAGE_COLOR_BLUE 0x93
#define MIDI_MESSAGE_ENDCODER 0xb0

enum Encoders
{
    ENCODER_PAN = 0x10,
    ENCODER_CLICK_PAN = 0x20,
    ENCODER_NAV = 0x3c,
    ENCODER_CLICK_NAV = 0x53,

};

Btn_Value ButtonOnBlinkOff(bool on, bool blink);

Btn_Value ButtonBlinkOnOff(bool blink, bool on);

#endif
