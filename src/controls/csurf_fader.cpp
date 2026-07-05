#include "csurf_fader.hpp"

#include "reaper_plugin_functions.h"

void CSurf_Fader::SendValue() const {
    if (m_midiout != nullptr) {
        m_midiout->Send(0xe0 + channel, value & 0x7f, (value >> 7) & 0x7f, -1);
    }
};

void CSurf_Fader::SetValue(const int _value, const bool force) {
    if (value == _value && !force) {
        return;
    }
    value = _value;
    this->SendValue();
};
