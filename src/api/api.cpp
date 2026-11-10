#include "api.hpp"

#include "reaper_vararg.hpp"
#include <reaper_plugin_functions.h>
#include "get_reasonus_version.hpp"
#include "fp_8_get_track_offset.hpp"
#include "fp_v2_get_controlled_track.hpp"
#include "fp_8_control_plugin.hpp"
#include "fp_8_control_thing_under_pointer.hpp"
#include "fp_8_set_track_filter_by_name.hpp"
#include "fp_8_set_fader_mode.hpp"
#include "fp_8_set_fader_mode_track.hpp"
#include "fp_8_set_fader_mode_plugin.hpp"
#include "fp_8_set_fader_mode_send.hpp"
#include "fp_8_set_fader_mode_receive.hpp"
#include "fp_8_set_fader_mode_pan.hpp"
#include "fp_8_set_track_filter.hpp"
#include "fp_8_set_track_filter_all.hpp"
#include "fp_8_set_track_filter_sends.hpp"
#include "fp_8_set_track_filter_receives.hpp"
#include "fp_8_set_track_filter_hw_outputs.hpp"
#include "fp_8_set_track_filter_instruments.hpp"
#include "fp_8_set_track_filter_effects.hpp"
#include "fp_8_set_track_filter_top_folders.hpp"
#include "fp_8_set_track_filter_all_folders.hpp"
#include "fp_8_set_track_filter_with_midi.hpp"
#include "fp_8_set_track_filter_with_audio.hpp"
#include "fp_8_set_track_filter_vca.hpp"

namespace REASONUS_API {
    void Register() {
        plugin_register("API_ReaSonus_GetVersion", reinterpret_cast<void *>(GetReasonusVersion));
        plugin_register("APIdef_ReaSonus_GetVersion", (void *) defstring_GetReasonusVersion);
        plugin_register("APIvararg_ReaSonus_GetVersion",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&GetReasonusVersion>));

        plugin_register("API_ReaSonus_FPv2_GetControlledTrack", reinterpret_cast<void *>(FPv2GetControlledTrack));
        plugin_register("APIdef_ReaSonus_FPv2_GetControlledTrack", (void *) defstring_FPv2GetControlledTrack);
        plugin_register("APIvararg_ReaSonus_FPv2_GetControlledTrack",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FPv2GetControlledTrack>));

        plugin_register("API_ReaSonus_FP8_GetTrackOffset", reinterpret_cast<void *>(FP8GetTrackOffset));
        plugin_register("APIdef_ReaSonus_FP8_GetTrackOffset", (void *) defstring_FP8GetTrackOffset);
        plugin_register("APIvararg_ReaSonus_FP8_GetTrackOffset",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8GetTrackOffset>));

        plugin_register("API_ReaSonus_FP8_ControlPlugin", reinterpret_cast<void *>(FP8ControlPlugin));
        plugin_register("APIdef_ReaSonus_FP8_ControlPlugin", (void *) defstring_FP8ControlPlugin);
        plugin_register("APIvararg_ReaSonus_FP8_ControlPlugin",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8ControlPlugin>));

        plugin_register("API_ReaSonus_FP8_ControlThingUnderPointer",
                        reinterpret_cast<void *>(FP8ControlThingUnderPointer));
        plugin_register("APIdef_ReaSonus_FP8_ControlThingUnderPointer", (void *) defstring_FP8ControlThingUnderPointer);
        plugin_register("APIvararg_ReaSonus_FP8_ControlThingUnderPointer",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8ControlThingUnderPointer>));

        plugin_register("API_ReaSonus_FP8_SetTrackFilterByName", reinterpret_cast<void *>(FP8SetTrackFilterByName));
        plugin_register("APIdef_ReaSonus_FP8_SetTrackFilterByName", (void *) defstring_FP8SetTrackFilterByName);
        plugin_register("APIvararg_ReaSonus_FP8_SetTrackFilterByName",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetTrackFilterByName>));

        plugin_register("API_ReaSonus_FP8_SetFaderMode", reinterpret_cast<void *>(FP8SetFaderMode));
        plugin_register("APIdef_ReaSonus_FP8_SetFaderMode", (void *) defstring_FP8SetFaderMode);
        plugin_register("APIvararg_ReaSonus_FP8_SetFaderMode",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetFaderMode>));

        plugin_register("API_ReaSonus_FP8_SetFaderModeTrack", reinterpret_cast<void *>(FP8SetFaderModeTrack));
        plugin_register("APIdef_ReaSonus_FP8_SetFaderModeTrack", (void *) defstring_FP8SetFaderModeTrack);
        plugin_register("APIvararg_ReaSonus_FP8_SetFaderModeTrack",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetFaderModeTrack>));

        plugin_register("API_ReaSonus_FP8_SetFaderModePlugin", reinterpret_cast<void *>(FP8SetFaderModePlugin));
        plugin_register("APIdef_ReaSonus_FP8_SetFaderModePlugin", (void *) defstring_FP8SetFaderModePlugin);
        plugin_register("APIvararg_ReaSonus_FP8_SetFaderModePlugin",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetFaderModePlugin>));

        plugin_register("API_ReaSonus_FP8_SetFaderModeSend", reinterpret_cast<void *>(FP8SetFaderModeSend));
        plugin_register("APIdef_ReaSonus_FP8_SetFaderModeSend", (void *) defstring_FP8SetFaderModeSend);
        plugin_register("APIvararg_ReaSonus_FP8_SetFaderModeSend",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetFaderModeSend>));

        plugin_register("API_ReaSonus_FP8_SetFaderModeReceive", reinterpret_cast<void *>(FP8SetFaderModeReceive));
        plugin_register("APIdef_ReaSonus_FP8_SetFaderModeReceive", (void *) defstring_FP8SetFaderModeReceive);
        plugin_register("APIvararg_ReaSonus_FP8_SetFaderModeReceive",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetFaderModeReceive>));

        plugin_register("API_ReaSonus_FP8_SetFaderModePan", reinterpret_cast<void *>(FP8SetFaderModePan));
        plugin_register("APIdef_ReaSonus_FP8_SetFaderModePan", (void *) defstring_FP8SetFaderModePan);
        plugin_register("APIvararg_ReaSonus_FP8_SetFaderModePan",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetFaderModePan>));

        plugin_register("API_ReaSonus_FP8_SetTrackFilter", reinterpret_cast<void *>(FP8SetTrackFilter));
        plugin_register("APIdef_ReaSonus_FP8_SetTrackFilter", (void *) defstring_FP8SetTrackFilter);
        plugin_register("APIvararg_ReaSonus_FP8_SetTrackFilter",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetTrackFilter>));

        plugin_register("API_ReaSonus_FP8_SetTrackFilterAll", reinterpret_cast<void *>(FP8SetTrackFilterAll));
        plugin_register("APIdef_ReaSonus_FP8_SetTrackFilterAll", (void *) defstring_FP8SetTrackFilterAll);
        plugin_register("APIvararg_ReaSonus_FP8_SetTrackFilterAll",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetTrackFilterAll>));

        plugin_register("API_ReaSonus_FP8_SetTrackFilterSends", reinterpret_cast<void *>(FP8SetTrackFilterSends));
        plugin_register("APIdef_ReaSonus_FP8_SetTrackFilterSends", (void *) defstring_FP8SetTrackFilterSends);
        plugin_register("APIvararg_ReaSonus_FP8_SetTrackFilterSends",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetTrackFilterSends>));

        plugin_register("API_ReaSonus_FP8_SetTrackFilterReceives", reinterpret_cast<void *>(FP8SetTrackFilterReceives));
        plugin_register("APIdef_ReaSonus_FP8_SetTrackFilterReceives", (void *) defstring_FP8SetTrackFilterReceives);
        plugin_register("APIvararg_ReaSonus_FP8_SetTrackFilterReceives",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetTrackFilterReceives>));

        plugin_register("API_ReaSonus_FP8_SetTrackFilterHwOutputs",
                        reinterpret_cast<void *>(FP8SetTrackFilterHwOutputs));
        plugin_register("APIdef_ReaSonus_FP8_SetTrackFilterHwOutputs", (void *) defstring_FP8SetTrackFilterHwOutputs);
        plugin_register("APIvararg_ReaSonus_FP8_SetTrackFilterHwOutputs",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetTrackFilterHwOutputs>));

        plugin_register("API_ReaSonus_FP8_SetTrackFilterInstruments",
                        reinterpret_cast<void *>(FP8SetTrackFilterInstruments));
        plugin_register("APIdef_ReaSonus_FP8_SetTrackFilterInstruments",
                        (void *) defstring_FP8SetTrackFilterInstruments);
        plugin_register("APIvararg_ReaSonus_FP8_SetTrackFilterInstruments",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetTrackFilterInstruments>));

        plugin_register("API_ReaSonus_FP8_SetTrackFilterEffects", reinterpret_cast<void *>(FP8SetTrackFilterEffects));
        plugin_register("APIdef_ReaSonus_FP8_SetTrackFilterEffects", (void *) defstring_FP8SetTrackFilterEffects);
        plugin_register("APIvararg_ReaSonus_FP8_SetTrackFilterEffects",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetTrackFilterEffects>));

        plugin_register("API_ReaSonus_FP8_SetTrackFilterTopFolders",
                        reinterpret_cast<void *>(FP8SetTrackFilterTopFolders));
        plugin_register("APIdef_ReaSonus_FP8_SetTrackFilterTopFolders", (void *) defstring_FP8SetTrackFilterTopFolders);
        plugin_register("APIvararg_ReaSonus_FP8_SetTrackFilterTopFolders",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetTrackFilterTopFolders>));

        plugin_register("API_ReaSonus_FP8_SetTrackFilterAllFolders",
                        reinterpret_cast<void *>(FP8SetTrackFilterAllFolders));
        plugin_register("APIdef_ReaSonus_FP8_SetTrackFilterAllFolders", (void *) defstring_FP8SetTrackFilterAllFolders);
        plugin_register("APIvararg_ReaSonus_FP8_SetTrackFilterAllFolders",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetTrackFilterAllFolders>));

        plugin_register("API_ReaSonus_FP8_SetTrackFilterWithMidi", reinterpret_cast<void *>(FP8SetTrackFilterWithMidi));
        plugin_register("APIdef_ReaSonus_FP8_SetTrackFilterWithMidi", (void *) defstring_FP8SetTrackFilterWithMidi);
        plugin_register("APIvararg_ReaSonus_FP8_SetTrackFilterWithMidi",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetTrackFilterWithMidi>));

        plugin_register("API_ReaSonus_FP8_SetTrackFilterWithAudio",
                        reinterpret_cast<void *>(FP8SetTrackFilterWithAudio));
        plugin_register("APIdef_ReaSonus_FP8_SetTrackFilterWithAudio", (void *) defstring_FP8SetTrackFilterWithAudio);
        plugin_register("APIvararg_ReaSonus_FP8_SetTrackFilterWithAudio",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetTrackFilterWithAudio>));

        plugin_register("API_ReaSonus_FP8_SetTrackFilterVca", reinterpret_cast<void *>(FP8SetTrackFilterVca));
        plugin_register("APIdef_ReaSonus_FP8_SetTrackFilterVca", (void *) defstring_FP8SetTrackFilterVca);
        plugin_register("APIvararg_ReaSonus_FP8_SetTrackFilterVca",
                        reinterpret_cast<void *>(&InvokeReaScriptAPI<&FP8SetTrackFilterVca>));
    }
}
