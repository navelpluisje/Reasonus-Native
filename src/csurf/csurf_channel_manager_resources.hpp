#ifndef CSURF_CHANNEL_MANAGER_RESOURCES_H_
#define CSURF_CHANNEL_MANAGER_RESOURCES_H_

#include "csurf_context.cpp"
#include "csurf_navigator.hpp"
#include "controls/csurf_button.hpp"
#include "csurf_track.hpp"
#include <vector>

/**
 * @brief Get a string representation of the pan value and the panMode. If panMode is 5, it will return the left pan representation
 *
 * @param pan The pan value
 * @param panMode The pan mode
 * @return string
 */
static string GetPanString(double pan, int panMode)
{
    int panInt = (int)(pan * 100.0);
    string strVal = to_string(abs(panInt));
    if (panMode == 5)
    {
        panInt = (panInt + 100) / 2;
        return to_string(panInt) + "L";
    }
    if (pan < 0)
    {
        return "L" + strVal;
    }
    if (pan > 0)
    {
        return "R" + strVal;
    }
    return "<C>";
}

/**
 * @brief Get a string representation of the width value and the panMode. If panMode is 5, it will return the right pan representation
 *
 * @param width The pan value
 * @param panMode The pan mode
 * @return string
 */
static string GetWidthString(double width, int panMode)
{
    int panInt = ((int)(width * 100.0) + 100) / 2;
    panInt = (panInt + 100) / 2;
    string strVal = to_string(abs(panInt));
    if (panMode == 5)
    {
        return to_string(panInt) + "R";
    }
    return "W" + strVal;
}

/**
 * @brief Get a string representation of the automtion mode
 *
 * @param automationMode The automation mode
 * @return string
 */
static string GetAutomationString(int automationMode)
{
    switch (automationMode)
    {
    case -1:
        return "Track";
    case 0:
        return "Trim";
    case 1:
        return "Read";
    case 2:
        return "Touch";
    case 3:
        return "Write";
    case 4:
        return "Latch";
    case 5:
        return "Preview";
    default:
        return "Trim";
    };
}

/**
 * @brief Get a string representation of the send mode
 *
 * @param sendMode The send mode
 * @return string
 */
static string GetSendModeString(int sendMode)
{
    switch (sendMode)
    {
    case 0:
        return "Post-fdr";
    case 1:
        return "Pre-FX";
    case 2:
    case 3:
        return "Post-FX";
    default:
        return "Post-fdr";
    };
}

enum ChannelMode
{
    TrackMode,
    PluginMode,
    SendMode,
    PanMode,
};

#endif