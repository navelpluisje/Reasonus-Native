#include "csurf_button_resources.hpp"

Btn_Value ButtonOnBlinkOff(bool on, bool blink, bool destraction_free)
{
    Btn_Value blink_value = destraction_free && blink ? BTN_VALUE_ON : BTN_VALUE_BLINK;

    return on ? BTN_VALUE_ON : blink ? blink_value
                                     : BTN_VALUE_OFF;
}

Btn_Value ButtonBlinkOnOff(bool blink, bool on, bool destraction_free)
{
    Btn_Value blink_value = destraction_free && blink ? BTN_VALUE_OFF : BTN_VALUE_BLINK;

    return blink
               ? blink_value
           : on ? BTN_VALUE_ON
                : BTN_VALUE_OFF;
}

Btn_Value ButtonConditionalBlink(bool blink, bool on)
{
    return on && blink ? BTN_VALUE_BLINK : on ? BTN_VALUE_ON
                                              : BTN_VALUE_OFF;
}
