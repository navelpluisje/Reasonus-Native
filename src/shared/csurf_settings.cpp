#ifndef CSURF_SETTINGS_H_
#define CSURF_SETTINGS_H_

#include "csurf_context_resources.hpp"
#include <cstdlib>
#include "csurf_utils.hpp"

class CSurf_Settings
{
    // Global settings
    /**
     * @brief Wether or not you want to control plugins with ReaSonus
     */
    bool plugin_control;
    /**
     * @brief Should the last touched parameter get 'untouched' after connectring it to a control
     */
    bool untouch_after_learn;
    /**
     * @brief Enable the last fader for master affter clicking themaster button
     */
    bool master_fader_mode;
    /**
     * @brief Swap the left and right shift button behaviuour
     */
    bool swap_shift_buttons;
    /**
     * @brief Use left-shit fader touch to reset the fader value
     */
    bool fader_reset;
    /**
     * @brief When engaged the mute and solo button behave as momentary buttons when pressing longer then 500ms
     */
    bool mute_solo_momentary;
    /**
     * @brief When engaged you use your own selected timeocode. Otherwise the selected time code in Reaper will be used
     */
    bool overwrite_time_code;
    /**
     * @brief When overwrite is true, this is the timecode used
     */
    int surface_time_code;
    /**
     * @brief The display type for the default track mode displays
     */
    int displays_track_display;
    /**
     * @brief Wether or not to be in distraction free mode
     */
    int distraction_free_mode;
    /**
     * @brief When overwrite is true, this is the timecode used
     */
    bool control_hidde_tracks;
    /**
     * @brief Wether or not the fader can be disabled
     */
    bool can_disable_fader;
    /**
     * @brief On the V2 go to the first track when pressing next on the last track en the other way around
     */
    bool endless_track_scroll;
    /**
     * @brief The the brightness of the button offstate for the track color
     */
    int track_color_brightness;
    /**
     * @brief When enabled pressing the latch preview when latch preview active trigger the action
     */
    bool latch_preview_action_enabled;
    /**
     * @brief action to perform for latch preview
     */
    int latch_preview_action_code;

public:
    CSurf_Settings() {}
    ~CSurf_Settings() {}

    /**************************************************************************
     * Settings
     **************************************************************************/
    void SetPluginControl(bool enabled)
    {
        plugin_control = enabled;
    }

    bool GetPluginControl()
    {
        return plugin_control;
    }

    void SetUntouchAfterLearn(bool enabled)
    {
        untouch_after_learn = enabled;
    }

    bool GetUntouchAfterLearn()
    {
        return untouch_after_learn;
    }

    void SetMasterFaderModeEnabled(bool enabled)
    {
        master_fader_mode = enabled;
    }

    bool GetMasterFaderModeEnabled()
    {
        return master_fader_mode;
    }

    void SetSwapShiftButtons(bool enabled)
    {
        swap_shift_buttons = enabled;
    }

    bool GetSwapShiftButtons()
    {
        return swap_shift_buttons;
    }

    void SetFaderReset(bool enabled)
    {
        fader_reset = enabled;
    }

    bool GetFaderReset()
    {
        return fader_reset;
    }

    void SetMuteSoloMomentary(bool enabled)
    {
        mute_solo_momentary = enabled;
    }

    bool GetMuteSoloMomentary()
    {
        return mute_solo_momentary;
    }

    void SetOverwriteTimeCode(bool enabled)
    {
        overwrite_time_code = enabled;
    }

    bool GetOverwriteTimeCode()
    {
        return overwrite_time_code;
    }

    void SetSurfaceTimeCode(int value)
    {
        surface_time_code = value;
    }

    bool GetSurfaceTimeCode()
    {
        return surface_time_code;
    }

    void SetTrackDisplay(int value)
    {
        displays_track_display = value;
    }

    int GetTrackDisplay()
    {
        return displays_track_display;
    }

    void SetControlHiddenTracks(bool value)
    {
        control_hidde_tracks = value;
    }

    bool GetControlHiddenTracks()
    {
        return control_hidde_tracks;
    }

    void SetCanDisableFader(bool value)
    {
        can_disable_fader = value;
    }

    bool GetCanDisableFader()
    {
        return can_disable_fader;
    }

    void SetDistractionFreeMode(bool value)
    {
        distraction_free_mode = value;
    };

    bool GetDistractionFreeMode()
    {
        return distraction_free_mode;
    };

    void SetEndlessTrackScroll(bool value)
    {
        endless_track_scroll = value;
    };

    bool GetEndlessTrackScroll()
    {
        return endless_track_scroll;
    };

    void SetTrackColorBrightness(int value)
    {
        track_color_brightness = value;
    };

    int GetTrackColorBrightness()
    {
        return track_color_brightness;
    };

    double GetTrackColorBrightnessPercentage()
    {
        return track_color_brightness / 100.0;
    };

    void SetLatchPreviewActionEnabled(bool value)
    {
        latch_preview_action_enabled = value;
    }

    bool GetLatchPreviewActionEnabled()
    {
        return latch_preview_action_enabled;
    }

    void SetLatchPreviewActionCode(int value)
    {
        latch_preview_action_code = value;
    }

    int GetLatchPreviewActionCode()
    {
        return latch_preview_action_code;
    }
};

#endif
