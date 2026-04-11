#ifndef CSURF_DISPLAY_RESOURCES_H_
#define CSURF_DISPLAY_RESOURCES_H_

static const int HEADER_LENGTH = 5;
static int SYSEX_HEADER[HEADER_LENGTH] = {0xf0, 0x00, 0x01, 0x06, 0x02};

static int SYSEX_START = 0xf0;
static int SYSEX_MANUFACTURER_1 = 0x00;
static int SYSEX_MANUFACTURER_2 = 0x01;
static int SYSEX_MANUFACTURER_3 = 0x06;
static int SYSEX_DEVICE_FP8 = 0x02;
static int SYSEX_DEVICE_FP16 = 0x16;
static int SYSEX_END = 0xf7;

enum DisplayActions {
    DISPLAY_ACTION_DISPLAY = 0x12,
    DISPLAY_ACTION_MODE = 0x13,
};

enum DisplayMode {
    /**
     * Small,
     * Small,
     * Large,
     * Value bar
     */
    DISPLAY_MODE_0 = 0x00,
    /**
     * Large,
     * Small,
     * Small,
     * Value bar
     */
    DISPLAY_MODE_1 = 0x01,
    /**
     * Small,
     * Small,
     * Small,
     * Small,
     * Value bar
     */
    DISPLAY_MODE_2 = 0x02,
    /**
     * Large,
     * Large,
     * Value bar
     */
    DISPLAY_MODE_3 = 0x03,
    /**
     * Large,
     * Large,
     * Value bar,
     * Vu Meter
     */
    DISPLAY_MODE_4 = 0x04,
    /**
     * Small,
     * Small,
     * Large,
     * Value bar,
     * Vu Meter
     */
    DISPLAY_MODE_5 = 0x05,
    /**
     * Small,
     * Large,
     * Small,
     * Value bar
     */
    DISPLAY_MODE_6 = 0x06,
    /**
     * Large,
     * Small,
     * Small,
     * Value bar,
     * Vu Meter
     */
    DISPLAY_MODE_7 = 0x07,
    /**
     * Small,
     * Large,
     * Small,
     * Value bar,
     * Vu Meter
     */
    DISPLAY_MODE_8 = 0x08,
    /**
     * Menu style,
     * 7 x tiny
     */
    DISPLAY_MODE_9 = 0x09,
};

enum Alignment {
    ALIGN_CENTER,
    ALIGN_LEFT,
    ALIGN_RIGHT,
};

enum Inverted {
    NON_INVERT = 0,
    INVERT = 4
};

enum DisplayValue {
    DISPLAY_VALUE_NAME,
    DISPLAY_VALUE_VOLUME,
    DISPLAY_VALUE_PAN_1,
    DISPLAY_VALUE_PAN_2,
    DISPLAY_VALUE_TRACK_NB,
    DISPLAY_VALUE_PHASE,
    DISPLAY_VALUE_ARMED,
    DISPLAY_VALUE_AUTOMATION,
    DISPLAY_VALUE_FX_STATE,
};


#endif
