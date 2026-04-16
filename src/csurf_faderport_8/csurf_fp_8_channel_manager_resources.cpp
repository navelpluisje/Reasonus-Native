#include <string>
#include "csurf_fp_8_channel_manager_resources.hpp"
#include <fmt/format.h>

#include "db2val.h"
#include "../shared/csurf_daw.hpp"
#include "../shared/csurf_utils.hpp"

/**
 * @brief Get a string representation of the pan value and the panMode. If panMode is 6, it will return the left pan representation
 *
 * @param pan The pan value
 * @param pan_mode The pan mode
 * @return string
 */
std::string GetPan1String(const double pan, const int pan_mode) {
    int pan_int;
    std::string str_val;

    if (pan_mode == PAN_MODE_DUAL_PAN) {
        pan_int = static_cast<int>(round((pan + 1) * 50.0));
        str_val = std::to_string(abs(pan_int));
        return str_val + "%L";
    }

    pan_int = static_cast<int>(round(pan * 100.0));
    str_val = std::to_string(abs(pan_int));

    if (pan_int < 0) {
        return str_val + "%L";
    }

    if (pan_int > 0) {
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
std::string GetPan2String(const double value, const int pan_mode) {
    if (pan_mode == PAN_MODE_DUAL_PAN) {
        const int pan_int = static_cast<int>(round((value + 1) * 50.0));
        const std::string str_val = std::to_string(abs(pan_int));

        return str_val + "%R";
    }

    if (pan_mode == PAN_MODE_STEREO_PAN) {
        const int pan_int = static_cast<int>(round(value * 100.0));
        const std::string str_val = std::to_string(pan_int);

        return str_val + "W";
    }

    return "";
}

std::string GetArmedString(bool armed) {
    return armed ? "Armed" : "-";
}

std::string GetPhaseString(const bool phase) {
    return phase ? "Phase On" : "Phase Off";
}

std::string GetTrackFXString(bool fx_on) {
    return fx_on ? "FX On" : "FX Off";
}


/**
 * @brief Get a string representation of the volume.
 *
 * @param volume The volume value
 * @return string
 */
std::string GetVolumeString(const double volume) {
    double value = VAL2DB(volume);
    int decimals = 0;

    if (abs(value) < 10) {
        decimals = 2;
    } else if (abs(value) < 100) {
        decimals = 1;
    }

    return fmt::format("{:.{}f}dB", value, decimals);
}
