#ifndef CSURF_DISPLAY_H_
#define CSURF_DISPLAY_H_

#include <reaper_plugin.h>
#include "csurf_display_resources.hpp"
#include <string>

using namespace std;

class CSurf_Display
{
protected:
    int channel;
    string values[4] = {"", "", "", ""};
    Inverted inverted[4] = {NON_INVERT, NON_INVERT, NON_INVERT, NON_INVERT};
    Alignment alignment[4] = {ALIGN_LEFT, ALIGN_LEFT, ALIGN_LEFT, ALIGN_LEFT};
    DisplayMode displayMode;

    midi_Output *m_midiout;

    void SendValue(int line);

    void SendMode();

public:
    CSurf_Display(int channel, midi_Output *m_midiout) : channel(channel), m_midiout(m_midiout) {};

    ~CSurf_Display() {};

    void SetValue(int line, Alignment _alignment, const char *_value, Inverted invert = NON_INVERT, bool force = false);

    void SetMode(DisplayMode _displayMode, bool force = false);
};

#endif // CSURF_DISPLAY_H_