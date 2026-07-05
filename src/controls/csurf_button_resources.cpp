#include "csurf_button_resources.hpp"

Btn_Value ButtonOnBlinkOff(const bool on, const bool blink, const bool destraction_free) {
    const Btn_Value blink_value = destraction_free && blink ? BTN_VALUE_ON : BTN_VALUE_BLINK;

    return on
               ? BTN_VALUE_ON
               : blink
                     ? blink_value
                     : BTN_VALUE_OFF;
}

Btn_Value ButtonBlinkOnOff(const bool blink, const bool on, const bool destraction_free) {
    const Btn_Value blink_value = destraction_free && blink ? BTN_VALUE_OFF : BTN_VALUE_BLINK;

    return blink
               ? blink_value
               : on
                     ? BTN_VALUE_ON
                     : BTN_VALUE_OFF;
}

Btn_Value ButtonConditionalBlink(const bool blink, const bool on) {
    return on && blink
               ? BTN_VALUE_BLINK
               : on
                     ? BTN_VALUE_ON
                     : BTN_VALUE_OFF;
}
