#ifndef CSURF_CONTEXT_RESOURCES_H_
#define CSURF_CONTEXT_RESOURCES_H_

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