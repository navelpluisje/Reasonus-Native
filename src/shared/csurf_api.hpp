#ifndef REASONUSNATIVE_CSURF_API_HPP
#define REASONUSNATIVE_CSURF_API_HPP

#include "./csurf_context.hpp"
#include "csurf_transport_manager.hpp"
#include "../csurf_faderport_8/csurf_fp_8_fader_manager.hpp"

class ReaSonusApi {
    CSurf_Context *context;
    CSurf_TransportManager *transportManager;
    CSurf_FP_8_FaderManager *faderManager;
    CSurf_FP_8_Navigator *trackNavigator;

    void SetPluginControl(int track_index, int plugin_index) const;

    void HandlePluginControl() const;

    void HandleSendMode() const;

    void HandlePanMode() const;

    void HandleFilterByName() const;

    void HandleFilterByType() const;

    void HandleSetFaderMode() const;

    void HandleTransportSeekSpeed() const;

    void HandleTransportSeekMode() const;

public:
    explicit ReaSonusApi(CSurf_Context *context, CSurf_TransportManager *transportManager);

    ReaSonusApi(
        CSurf_Context *context,
        CSurf_TransportManager *transportManager,
        CSurf_FP_8_FaderManager *faderManager,
        CSurf_FP_8_Navigator *trackNavigator);

    ~ReaSonusApi();

    void HandleApi(const std::string &device) const;
};

#endif //REASONUSNATIVE_CSURF_API_HPP
