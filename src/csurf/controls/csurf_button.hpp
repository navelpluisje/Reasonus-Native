#ifndef CSURF_BUTTON_H_
#define CSURF_BUTTON_H_

#include <reaper_plugin.h>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include "csurf_button_resources.hpp"

class CSurf_Button
{
protected:
    Btn_Value value;
    Btn_Types type;
    midi_Output *m_midiout;

    void SendValue();

public:
    CSurf_Button(Btn_Types type, Btn_Value value, midi_Output *m_midiout);

    ~CSurf_Button() {};

    virtual void SetValue(Btn_Value value, bool force = false);
};

#endif // CSURF_BUTTON_H_