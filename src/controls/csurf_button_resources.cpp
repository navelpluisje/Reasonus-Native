#include "csurf_button_resources.hpp"

Btn_Value ButtonOnBlinkOff(bool on, bool blink)
{
    return on ? BTN_VALUE_ON : blink ? BTN_VALUE_BLINK
                                     : BTN_VALUE_OFF;
}

Btn_Value ButtonBlinkOnOff(bool blink, bool on)
{
    return blink ? BTN_VALUE_BLINK : on ? BTN_VALUE_ON
                                        : BTN_VALUE_OFF;
}
