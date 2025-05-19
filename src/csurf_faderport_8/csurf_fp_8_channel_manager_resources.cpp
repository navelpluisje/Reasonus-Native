#include <string>
#include "csurf_fp_8_channel_manager_resources.hpp"
#include "../shared/csurf_context.cpp"
#include "csurf_fp_8_navigator.hpp"
#include "../controls/csurf_button.hpp"
#include "csurf_fp_8_track.hpp"

/**
 * @brief Get a string representation of the pan value and the panMode. If panMode is 5, it will return the left pan representation
 *
 * @param pan The pan value
 * @param panMode The pan mode
 * @return string
 */
std::string GetPanString(double pan)
{
    int panInt = (int)(pan * 100.0);
    std::string strVal = std::to_string(abs(panInt));
    if (panInt < 0)
    {
        return strVal + "%L";
    }
    if (panInt > 0)
    {
        return strVal + "%R";
    }
    return "Center";
}

/**
 * @brief Get a string representation of the width value and the panMode. If panMode is 5, it will return the right pan representation
 *
 * @param width The pan value
 * @param panMode The pan mode
 * @return string
 */
std::string GetWidthString(double width, int panMode)
{
    if (panMode == 6)
    {
        return GetPanString(width);
    }
    int panInt = (int)(width * 100.0);
    std::string strVal = std::to_string(panInt);
    return strVal + "W";
}
