#ifndef CSURF_CONTEXT_H_
#define CSURF_CONTEXT_H_

class CSurf_Context
{
    bool shift_left;
    bool shift_right;
    bool arm;

public:
    CSurf_Context() {}
    ~CSurf_Context() {}

    void SetShiftLeft(bool val) { shift_left = val; }
    bool GetSfiftLeft() { return shift_left; }

    void SetShiftRight(bool val) { shift_right = val; }
    bool GetSfiftRight() { return shift_right; }

    void SetArm(bool val) { arm = val; }
    bool GetArm() { return arm; }
};

#endif
