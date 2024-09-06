#ifndef CSURF_BUTTON_H_
#define CSURF_BUTTON_H_

#include <reaper_plugin.h>
#include <reaper_plugin_functions.h>
#include "csurf_button_resources.hpp"

using namespace std;

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

    void SetValue(Btn_Value value);
};

#endif // CSURF_BUTTON_H_