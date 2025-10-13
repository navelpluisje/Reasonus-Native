#include "./csurf_osara.hpp"
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include "csurf_utils.hpp"
#include <functional>

CSurfOsara &CSurfOsara::GetInstance()
{
    static CSurfOsara instance; // Guaranteed to be destroyed
    return instance;            // Instantiated on first use
}

void CSurfOsara::Speak(std::string message)
{
    logInteger("Test", 42);
    ShowConsoleMsg(("Message: " + message + "\n").c_str());

    if (osara_outputMessage)
    {
        ShowConsoleMsg("Speak");
        osara_outputMessage(message.c_str());
    }
    else
    {
        ShowConsoleMsg("Speachless");
    }
}

CSurfOsara::CSurfOsara()
{
    ShowConsoleMsg("Constructor\n");
    test = 42;

    osara_outputMessage = (decltype(osara_outputMessage))plugin_getapi("osara_outputMessage");

    if (osara_outputMessage)
    {
        osara_outputMessage("Screeeeeaaaaaammmmmmmmmmm");
        ShowConsoleMsg("Dis you hear me?\n");
    }
}
