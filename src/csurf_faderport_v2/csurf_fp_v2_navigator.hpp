#ifndef CSURF_NAVIGATOR_V2_H_
#define CSURF_NAVIGATOR_V2_H_

#include "../shared/csurf_context.cpp"
#include <WDL/ptrlist.h>

class CSurf_FP_V2_Navigator
{
    int track_offset = 0;
    CSurf_Context *context;
    ReaSonusSettings *settings = ReaSonusSettings::GetInstance(FP_V2);

    WDL_PtrList<MediaTrack> tracks;
    bool hasSolo;
    bool hasMute;
    bool hasArmed;
    bool hasAllArmed;
    bool isTouched;

    void UpdateMixerPosition();

    void GetAllControllableTracks(WDL_PtrList<MediaTrack> &tracks, bool &hasSolo, bool &hasMute);

public:
    CSurf_FP_V2_Navigator(CSurf_Context *context);

    MediaTrack *GetControllerTrack();

    bool IsTrackTouched(const MediaTrack *media_track, int is_pan);

    void SetOffset(int offset);

    void SetOffsetByTrack(MediaTrack *media_track);

    int GetOffset() const;

    void IncrementOffset(int count);

    void DecrementOffset(int count);

    void UpdateOffset();

    MediaTrack *GetNextTrack();

    MediaTrack *GetPreviousTrack();

    bool HasTracksWithSolo() const;

    bool HasTracksWithMute() const;

    bool HasArmedTracks() const;

    bool HasAllArmedTracks() const;

    void SetIsTouched(bool value);
};

#endif
