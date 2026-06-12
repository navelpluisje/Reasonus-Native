#ifndef CSURF_COLOR_BUTTON_COLORS_H_
#define CSURF_COLOR_BUTTON_COLORS_H_
#include "../shared/csurf_utils.hpp"

struct ButtonColor {
    int red;
    int green;
    int blue;

    void SetColor(const int _red, const int _green, const int _blue) {
        red = _red;
        green = _green;
        blue = _blue;
    }

    void SetColor(const ButtonColor color) {
        red = color.red;
        green = color.green;
        blue = color.blue;
    }

    /**
     * Create color from color picker value.
     * The color picker  has an option for alpha (0xRRGGBBAA) and no alpha (0xXXRRGGBB)
     * @param color The integer interpretation of the color
     * @param has_alpha Whter the value has alpha in it
     */
    void SetColor(const int color, const bool has_alpha) {
        if (has_alpha) {
            red = (color & 0xff000000) >> 24;
            green = (color & 0x00ff0000) >> 16;
            blue = (color & 0x0000ff00) >> 8;
        } else {
            red = (color & 0x00ff0000) >> 16;
            green = (color & 0x0000ff00) >> 8;
            blue = color & 0x000000ff;
        }
    }

    [[nodiscard]] bool IsColor(const ButtonColor color) const {
        return color.blue == blue && color.red == red && color.green == green;
    }
};

static ButtonColor ButtonColorWhite = {0x7f, 0x7f, 0x7f};
static ButtonColor ButtonColorWhiteDim = {0x7f, 0x7f, 0x7f};

static ButtonColor ButtonColorRed = {0x7f, 0x00, 0x00};
static ButtonColor ButtonColorRedDim = {0x7f, 0x00, 0x00};

static ButtonColor ButtonColorGreen = {0x00, 0x7f, 0x00};
static ButtonColor ButtonColorGreenDim = {0x00, 0x7f, 0x00};

static ButtonColor ButtonColorYellow = {0x7f, 0x7f, 0x00};
static ButtonColor ButtonColorYellowDim = {0x7f, 0x7f, 0x00};

static ButtonColor ButtonColorPurple = {0x1b, 0x00, 0x7f};
static ButtonColor ButtonColorPurpleDim = {0x1b, 0x00, 0x7f};

static ButtonColor ButtonColorBlue = {0x00, 0x1b, 0x7f};
static ButtonColor ButtonColorBlueDim = {0x00, 0x1b, 0x7f};

static ButtonColor ButtonColorBlack = {0x00, 0x00, 0x00};

#endif // CSURF_COLOR_BUTTON_COLORS_H_
