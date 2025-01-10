#include "csurf_display.hpp"
#include <reaper_plugin.h>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include "../csurf_utils.hpp"

void CSurf_Display::SendValue(int line)
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

    if (m_midiout)
    {
        m_midiout->SendMsg(&midiSysExData.evt, 0);
    }
};

void CSurf_Display::SendMode()
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

    if (m_midiout)
    {
        m_midiout->SendMsg(&midiSysExData.evt, 0);
    }
};

void CSurf_Display::SetValue(int line, Alignment _alignment, const char *_value, Inverted invert, bool force)
{
    // We have to convert the char array to a string. Somehow the simple conversion makes it crash
    char buffer[250];
    snprintf(buffer, sizeof(buffer), "%s", _value);
    string strVal = buffer;

    // If values have not changed, we do nothing
    if (strVal == values[line] && alignment[line] == _alignment && inverted[line] == invert && !force)
    {
        return;
    }
    values[line] = strVal;
    alignment[line] = _alignment;
    inverted[line] = invert;
    this->SendValue(line);
};

void CSurf_Display::SetMode(DisplayMode _displayMode, bool force)
{
    if (displayMode == _displayMode && !force)
    {
        return;
    }
    displayMode = _displayMode;
    this->SendMode();
};
