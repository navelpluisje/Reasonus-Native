#ifndef CSURF_CONTEXT_RESOURCES_H_
#define CSURF_CONTEXT_RESOURCES_H_

/**
 * @brief Used for the trackmode for the FaderPort 8 & 16
 */
enum ChannelMode
{
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

enum ChannelManagerType
{
    Track,
    Hui,
};

enum PanEncoderMode
{
    PanEncoderTrackPanMode,
    PanEncoderPluginMode,
    PanEncoderSendMode,
    PanEncoderReceiveMode,
};

#endif // CSURF_CONTEXT_RESOURCES_H_