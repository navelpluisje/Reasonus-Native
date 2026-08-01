//
// Created by Erwin Goossen on 31/07/2026.
//

// ReSharper disable CppWrongIncludesOrder
#ifndef REASONUSNATIVE_CSURF_SINGLE_POINT_AUTOMATION_ITEM_HPP
#define REASONUSNATIVE_CSURF_SINGLE_POINT_AUTOMATION_ITEM_HPP

// ReSharper disable once CppUnusedIncludeDirective
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include "csurf_daw.hpp"

struct SinglePointAutomationItem {
private:
    TrackEnvelope *envelope;
    double start_position;
    double end_position;

public:
    SinglePointAutomationItem(MediaTrack *media_track, const std::string &chunk_name, const double value) {
        start_position = GetPlayPosition();
        envelope = DAW::GetTrackEnvelopeByChunkName(
            media_track,
            start_position,
            chunk_name,
            value
        );
        end_position = -1;
    }

    void SetEndPosition(const double end_pos) {
        end_position = end_pos;
    }

    [[nodiscard]] TrackEnvelope *GetEnvelope() const {
        return envelope;
    }

    [[nodiscard]] double GetStartPosition() const {
        return start_position;
    }

    [[nodiscard]] double GetEndPosition() const {
        return end_position;
    }

    void InsertStartPoint(const double value) {
        start_position = GetPlayPosition();
        DAW::DisableEnvelope(envelope);
        DAW::InsertEnvelopePoint(
            envelope,
            start_position,
            value
        );
    }

    void InsertEndPoint(const double value) {
        end_position = GetPlayPosition();
        DAW::EnableEnvelope(envelope);
        DAW::InsertEnvelopePoint(
            envelope,
            end_position,
            value
        );
        DeleteRange();
    }

    void DeleteRange() const {
        DeleteEnvelopePointRange(envelope, start_position + 0.000001, end_position - 0.000001);
    }
};

#endif //REASONUSNATIVE_CSURF_SINGLE_POINT_AUTOMATION_ITEM_HPP
