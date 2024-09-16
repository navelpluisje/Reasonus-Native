#ifndef CSURF_COLOR_BUTTON_COLORS_H_
#define CSURF_COLOR_BUTTON_COLORS_H_

struct ButtonColor
{
    int red;
    int green;
    int blue;

    void SetColor(int _red, int _green, int _blue)
    {
        red = _red;
        green = _green;
        blue = _blue;
    }

    bool IsColor(ButtonColor color)
    {
        return color.blue == blue && color.red == red && color.green == green;
    }
};

static ButtonColor ButtonColorWhite = {0x7f, 0x7f, 0x7f};
static ButtonColor ButtonColorWhiteDim = {0x0a, 0x0a, 0x0a};

static ButtonColor ButtonColorRed = {0x7f, 0x00, 0x00};
static ButtonColor ButtonColorRedDim = {0x0a, 0x00, 0x00};

static ButtonColor ButtonColorGreen = {0x00, 0x7f, 0x00};
static ButtonColor ButtonColorGreenDim = {0x00, 0x0a, 0x00};

static ButtonColor ButtonColorYellow = {0x7f, 0x7f, 0x00};
static ButtonColor ButtonColorYellowDim = {0x0a, 0x0a, 0x00};

static ButtonColor ButtonColorPurple = {0x1b, 0x00, 0x7f};
static ButtonColor ButtonColorPurpleDim = {0x03, 0x00, 0x0a};

static ButtonColor ButtonColorBlue = {0x00, 0x1b, 0x7f};
static ButtonColor ButtonColorBlueDim = {0x00, 0x03, 0x0a};

static ButtonColor ButtonColorBlack = {0x00, 0x00, 0x00};

#endif // CSURF_COLOR_BUTTON_H_