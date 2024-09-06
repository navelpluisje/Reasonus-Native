#ifndef CSURF_UTILS_H_
#define CSURF_UTILS_H_

#include <db2val.h>
#include <reaper_plugin_functions.h>

/**
 * @brief Get the normalized value of the volume to send to the device faders
 *
 * @param vol The volume to normalize
 * @return double Normalized value between 0.0 - 1.0
 */
static double volToNormalized(double vol)
{
    double d = (DB2SLIDER(VAL2DB(vol)) / 1000.0);
    if (d < 0.0)
    {
        return 0.0;
    }

    if (d > 1.0)
    {
        return 1.0;
    }

    return d;
}

#endif // CSURF_UTILS_H_
