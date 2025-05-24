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
    PanMode,
    TrackPanMode,
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
    PanEncoderPanMode,
    PanEncoderPluginMode,
    PanEncoderSendMode,
    PanEncoderReceiveMode,
};

#endif // CSURF_CONTEXT_RESOURCES_H_