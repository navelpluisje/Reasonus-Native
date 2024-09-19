#include "csurf_context_resources.hpp"
#ifndef CSURF_CONTEXT_H_
#define CSURF_CONTEXT_H_

class CSurf_Context
{
    bool shift_left;
    bool shift_right;
    bool arm;
    bool lastTouchedFxMode;
    PanEncoderMode panEncoderMode = PanEncoderPanMode;

public:
    CSurf_Context()
    {
    }
    ~CSurf_Context() {}

    void SetShiftLeft(bool val) { shift_left = val; }
    bool GetShiftLeft() { return shift_left; }

    void SetShiftRight(bool val) { shift_right = val; }
    bool GetShiftRight() { return shift_right; }

    void SetArm(bool val) { arm = val; }
    bool GetArm() { return arm; }

    void SetPanEncoderMode(PanEncoderMode val) { panEncoderMode = val; }
    PanEncoderMode GetPanEncoderMode() { return panEncoderMode; }

    void ToggleLastTouchedFxMode() { lastTouchedFxMode = !lastTouchedFxMode; }
    bool GetLastTouchedFxMode() { return lastTouchedFxMode; }
};

#endif
