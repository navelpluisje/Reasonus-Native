#include <string>
#include <cmath>
#include "csurf_fp_8_channel_manager_resources.hpp"
#include "../shared/csurf_context.cpp"
#include "../shared/csurf_daw.hpp"
#include "csurf_fp_8_navigator.hpp"
#include "../controls/csurf_button.hpp"
#include "csurf_fp_8_track.hpp"

/**
 * @brief Get a string representation of the pan value and the panMode. If panMode is 6, it will return the left pan representation
 *
 * @param value The pan value
 * @param pan_mode The pan mode
 * @return string
 */
std::string GetPan1String(double value, int pan_mode)
{
    int pan_int;
    std::string str_val;

    if (pan_mode == PAN_MODE_DUAL_PAN)
    {
        pan_int = (int)round((value + 1) * 50.0);
        str_val = std::to_string(abs(pan_int));
        return str_val + "%L";
    }

    pan_int = (int)round(value * 100.0);
    str_val = std::to_string(abs(pan_int));
    if (pan_int < 0)
    {
        return str_val + "%L";
    }
    if (pan_int > 0)
    {
        return str_val + "%R";
    }
    return "Center";
}

/**
 * @brief Get a string representation of the width value and the panMode.
 * If panMode is 6, it will return the right pan representation
 * If panMode is 5, it will return the width representation
 *
 * @param value The pan value
 * @param pan_mode The pan mode
 * @return string
 */
std::string GetPan2String(double value, int pan_mode)
{
    int pan_int;
    std::string str_val;

    if (pan_mode == PAN_MODE_DUAL_PAN)
    {
        pan_int = (int)round((value + 1) * 50.0);
        str_val = std::to_string(abs(pan_int));
        return str_val + "%R";
    }

    if (pan_mode == PAN_MODE_STEREO_PAN)
    {
        int pan_int = (int)round(value * 100.0);
        std::string str_val = std::to_string(pan_int);
        return str_val + "W";
    }

    return "";
}
