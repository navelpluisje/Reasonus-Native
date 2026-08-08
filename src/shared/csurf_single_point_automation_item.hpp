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

enum SinglePointAutomationType {
    SPA_Track,
    SPA_Send,
    SPA_Plugin,
};

struct SinglePointAutomationItem {
private:
    TrackEnvelope *envelope;
    double start_position;
    double end_position;
    std::string chunk_name;
    SinglePointAutomationType type;

    /**
     * Delete the points between the start point and the endpoint
     */
    void DeleteRange() const {
        DeleteEnvelopePointRange(envelope, start_position + 0.000001, end_position - 0.000001);
    }

    /**
     * Delete the start point of the envelope
     */
    void DeleteStartPoint() const {
        DeleteEnvelopePointRange(envelope, start_position, start_position);
    }

public:
    SinglePointAutomationItem(
        MediaTrack *media_track,
        const std::string &chunk_name,
        const double value,
        const SinglePointAutomationType spa_type
    ) : chunk_name(chunk_name), type(spa_type) {
        start_position = GetPlayPosition();

        envelope = DAW::GetTrackEnvelopeByChunkName(
            media_track,
            start_position,
            chunk_name,
            value
        );

        end_position = -1;
    }

    SinglePointAutomationItem(
        MediaTrack *media_track,
        const std::string &chunk_name,
        const double value,
        const SinglePointAutomationType spa_type,
        const int item_index
    ) : chunk_name(chunk_name), type(spa_type) {
        start_position = GetPlayPosition();
        if (spa_type == SPA_Send) {
            envelope = DAW::GetTrackSendEnvelope(media_track, item_index, start_position, chunk_name, value);
        }

        end_position = -1;
    }

    /**
     * Set the start position of the envelope.
     * @param value The start value for this envelope
     */
    void InsertStartPoint(const double value) {
        start_position = GetPlayPosition();
        DAW::DisableEnvelope(envelope);
        DAW::InsertEnvelopePoint(
            envelope,
            start_position,
            value
        );
    }

    /**
     * Insert the endpoint for the single point automation. There is a check performed prior adding the point to the envelope:
     * - The chunk name has to be the same as the start chunk name. As this one differs from theinitial one,
     * we'll remove the start_position as well as someting unexpected happened
     * @param value The value for the end point
     * @param chunk The chunk name to set it for. As this one differs from theinitial one,
     * we'll remove the start_position as well as someting unexpected happened
     */
    void InsertEndPoint(const double value, const std::string &chunk) {
        DAW::EnableEnvelope(envelope);
        if (chunk != chunk_name) {
            DeleteStartPoint();
            return;
        }

        end_position = GetPlayPosition();
        DAW::InsertEnvelopePoint(
            envelope,
            end_position,
            value
        );
        DeleteRange();
    }
};

#endif //REASONUSNATIVE_CSURF_SINGLE_POINT_AUTOMATION_ITEM_HPP
