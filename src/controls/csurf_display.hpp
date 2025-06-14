#ifndef CSURF_DISPLAY_H_
#define CSURF_DISPLAY_H_

#include <reaper_plugin.h>
#include "csurf_display_resources.hpp"
#include <string>

class CSurf_Display
{
protected:
    int channel;
    std::string values[7] = {"", "", "", "", "", "", ""};
    Inverted inverted[7] = {NON_INVERT, NON_INVERT, NON_INVERT, NON_INVERT, NON_INVERT, NON_INVERT, NON_INVERT};
    Alignment alignment[7] = {ALIGN_LEFT, ALIGN_LEFT, ALIGN_LEFT, ALIGN_LEFT, ALIGN_LEFT, ALIGN_LEFT, ALIGN_LEFT};
    DisplayMode displayMode;
    midi_Output *m_midiout;
    int device_id;

    void SendValue(int line);

    void SendMode();

public:
    CSurf_Display(int channel, midi_Output *m_midiout, int device_id) : channel(channel), m_midiout(m_midiout), device_id(device_id) {};

    ~CSurf_Display() {};

    void SetValue(int line, Alignment _alignment, const char *_value, Inverted invert = NON_INVERT, bool force = false);

    void SetMode(DisplayMode _displayMode, bool force = false);
};

#endif // CSURF_DISPLAY_H_