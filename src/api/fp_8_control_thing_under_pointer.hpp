#pragma once
#include <regex>
#include <string>
#include <vector>

#include "../../lib/reaper-sdk/sdk/reaper_plugin_functions.h"
#include "../shared/csurf.h"
#include "../shared/csurf_context_resources.hpp"
#include "../shared/csurf_utils.hpp"

namespace REASONUS_API {
    inline void FP8ControlThingUnderPointer() {
        char info_buffer[256];
        int pointer_x = 0;
        int pointer_y = 0;

        // Get the current mouseposition
        GetMousePosition(&pointer_x, &pointer_y);
        // Get the media track and the relevant info for the given mouse position
        MediaTrack *media_track = GetThingFromPoint(pointer_x, pointer_y, info_buffer, sizeof info_buffer);
        // Get the track index (zero-based)
        const int track_index = stoi(DAW::GetTrackIndex(media_track)) - 1;

        // Cast the info buffer to a string to make it easier to get data from
        const std::string pointer_info = info_buffer;

        if (std::regex_match(pointer_info, std::regex("(tcp|mcp).(fxlist\\s[0-9]{1,5}\\sfx:[0-9]{1,5})")) ||
            std::regex_match(pointer_info, std::regex("(tcp|mcp).(fxparm\\s[0-9]{1,5}\\sfx:[0-9]{1,5})"))) {
            const std::vector<std::string> pointer_info_parts = split(pointer_info, ":");
            std::string plugin_index = pointer_info_parts.size() > 1 ? pointer_info_parts.back() : "";

            // If the plugin index is empty, clear the extState, otherwise set the track index and plugin index
            if (!plugin_index.empty()) {
                DAW::SetExtState(EXT_STATE_FP_PLUGIN_CONTROL, fmt::format("{}:{}", track_index, plugin_index), false);
            } else {
                DAW::SetExtState(EXT_STATE_FP_PLUGIN_CONTROL, "true", false);
            }

            return;
        }

        // Check if the pointer is above the tcp or mcp's fx list
        if (std::regex_match(pointer_info, std::regex("(tcp|mcp).(fx)(.*)"))) {
            DAW::SetExtState(EXT_STATE_FP_PLUGIN_CONTROL, "true", false);
            return;
        }

        // Check if the pointer is above the tcp or mcp's send list
        if (std::regex_match(pointer_info, std::regex("(tcp|mcp).(sendlist|io)(.*)"))) {
            DAW::SetExtState(EXT_STATE_FP_SENDS_CONTROL, "true", false);
            return;
        }

        // Check if the pointer is above the tcp or mcp's pan control
        if (std::regex_match(pointer_info, std::regex("(tcp|mcp).(pan|width)(.*)"))) {
            DAW::SetExtState(EXT_STATE_FP_PAN_CONTROL, "true", false);
            return;
        }

        // Check if the pointer is above the tcp or mcp's volume control
        if (std::regex_match(pointer_info, std::regex("(tcp|mcp).(volume)(.*)"))) {
            // Switch to track mode
            DAW::SetExtState(EXT_STATE_FP_FADER_MODE, std::to_string(TrackMode), false);
            return;
        }

        // Check if the pointer is above the tcp or mcp's area without controls
        if (std::regex_match(pointer_info, std::regex("(tcp|mcp)"))) {
            // Select a track
            DAW::SetUniqueSelectedTrack(media_track);
            return;
        }

        DAW::SetExtState(EXT_STATE_FP_PLUGIN_CONTROL, "", false);
    }

    inline auto defstring_FP8ControlThingUnderPointer =
            "void\0\0\0"
            "Trigger action under mouse pointer for both mcp and tcp:\n"
            "  - Plugin list, filled slot: Start controlling plugin from the controller\n"
            "  - Plugin list, empty slot: Open Plugin mode on the controller\n"
            "  - FX select button: Open Plugin mode on the controller\n"
            "  - Sends list, any slot: Open Sends mode on the controller\n"
            "  - Pan or Width control: Open Pan mode on the controller\n"
            "  - Volume control: Open Track mode on the controller\n"
            "  - Any other point in mcp/tcp: Select a track\n";
}
