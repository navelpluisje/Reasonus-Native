#ifndef CSURF_CONTEXT_RESOURCES_H_
#define CSURF_CONTEXT_RESOURCES_H_

/**
 * @brief Used for the trackmode for the FaderPort 8 & 16
 */
enum ChannelMode {
    TrackMode,
    PluginMode,
    TrackPluginMode,
    SendMode,
    TrackSendMode,
    ReceiveMode,
    TrackReceiveMode,
    PanMode1,
    PanMode2,
    MixMode,
    PluginEditMode,
    PluginControlMode,
    MenuMode
};

enum ChannelManagerType {
    Track,
    Hui,
};

enum PanEncoderMode {
    PanEncoderTrackPanMode,
    PanEncoderPluginMode,
    PanEncoderSendMode,
    PanEncoderReceiveMode,
    PanEncoderPanMode,
    PanEncoderMixMode,
    PanEncoderPluginEditMode,
    PanEncoderPluginControlMode,
    PanEncoderPluginStepSizeMode,
    PanEncoderMenuMode,
};

enum NavigatorFilter {
    /**
     * @brief Select all tracks in the project
     */
    TrackAllFilter,
    /**
     * @brief Select all tracks that have sends
     */
    TrackSendsFilter,
    /**
     * @brief Select all tracks that have receives
     */
    TrackReceivesFilter,
    /**
     * @brief Select all tracks that have a hardware output
     */
    TrackHarwareOutputFilter,
    /**
     * @brief Select all tracks that have a virtual instrument
     */
    TrackInstrumentsFilter,
    /**
     * @brief Select all tracks that have one or more effects
     */
    TrackEffectsFilter,
    /**
     * @brief Select all tracks that are a folder on top level
     */
    TrackTopFoldersFilter,
    /**
     * @brief Select all tracks that are a folder
     */
    TrackAllFoldersFilter,
    /**
     * @brief Select all tracks that have a media item with MIDI
     */
    TrackWithMidiFilter,
    /**
     * @brief Select all tracks that have a media item with Audio
     */
    TrackWithAudioFilter,
    /**
     * @brief Select all tracks that are a VCA Lead
     */
    TrackIsVcaFilter,
    TrackCustomFilter,
    TrackCustomMultiSelectFilter,
};

#endif // CSURF_CONTEXT_RESOURCES_H_
