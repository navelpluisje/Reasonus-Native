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

    void SendValue(int line)
    {
        int align = 0x0000000 + inverted[line] + alignment[line];

        struct
        {
            MIDI_event_t evt;
            char data[256];
        } midiSysExData;

        midiSysExData.evt.frame_offset = 0;
        midiSysExData.evt.size = 0;
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = SYSEX_HEADER[0];
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = SYSEX_HEADER[1];
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = SYSEX_HEADER[2];
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = SYSEX_HEADER[3];
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = SYSEX_HEADER[4];
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = DISPLAY_ACTION_DISPLAY;
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = channel;
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = line;
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = align;

        int length = (int)strlen(values[line].c_str());

        if (length > 10)
            length = 10;

        int count = 0;

        while (count < length)
        {
            midiSysExData.evt.midi_message[midiSysExData.evt.size++] = values[line][count]; // tx text in ASCII format
            count++;
        }

        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = 0xF7;

        m_midiout->SendMsg(&midiSysExData.evt, 0);
    };

    void SendMode()
    {
        struct
        {
            MIDI_event_t evt;
            char data[256];
        } midiSysExData;

        midiSysExData.evt.frame_offset = 0;
        midiSysExData.evt.size = 0;
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = SYSEX_HEADER[0];
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = SYSEX_HEADER[1];
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = SYSEX_HEADER[2];
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = SYSEX_HEADER[3];
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = SYSEX_HEADER[4];
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = DISPLAY_ACTION_MODE;
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = channel;
        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = 0x00 + displayMode;

        midiSysExData.evt.midi_message[midiSysExData.evt.size++] = 0xF7;

        m_midiout->SendMsg(&midiSysExData.evt, 0);
    };

public:
    CSurf_Display(int channel, midi_Output *m_midiout) : channel(channel), m_midiout(m_midiout) {};

    ~CSurf_Display() {};

    void SetValue(int line, Alignment _alignment, const char *_value, Inverted invert = NON_INVERT)
    {
        // We have to convert the char array to a string. Somehow the simple conversion makes it crash
        char buffer[250];
        snprintf(buffer, sizeof(buffer), "%s", _value);
        string strVal = buffer;

        // If values have not changed, we do nothing
        if (strVal == values[line] && alignment[line] == _alignment)
        {
            return;
        }
        values[line] = strVal;
        alignment[line] = _alignment;
        inverted[line] = invert;
        this->SendValue(line);
    };

    void SetMode(DisplayMode _displayMode)
    {
        if (displayMode == _displayMode)
        {
            return;
        }
        displayMode = _displayMode;
        this->SendMode();
    };
};

#endif // CSURF_DISPLAY_H_