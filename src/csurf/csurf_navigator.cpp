#ifndef CSURF_NAVIGATOR_C_
#define CSURF_NAVIGATOR_C_

#include <WDL/ptrlist.h>
#include <reaper_plugin.h>
#include <reaper_plugin_functions.h>

class CSurf_Navigator
{
    int nb_channels = 8;
    int track_offset = 0;

    WDL_PtrList<MediaTrack> tracks;

    void GetAllTracks()
    {
        tracks.Empty();

        for (int i = 0; i < CountTracks(0); i++)
        {
            tracks.Add(GetTrack(0, i));
        }
    }

public:
    CSurf_Navigator() {

    };

    MediaTrack *GetTrackByIndex(int index)
    {
        WDL_PtrList<MediaTrack> bank = GetBankTracks();
        return bank.Get(index);
    }

    WDL_PtrList<MediaTrack> GetBankTracks()
    {
        WDL_PtrList<MediaTrack> bank;

        GetAllTracks();
        for (int i = track_offset; i < track_offset + nb_channels; i++)
        {
            bank.Add(GetTrack(0, i));
        }
        return bank;
    }

    void AdjustBankOffset(int offset)
    {
        track_offset += offset;
    }
};

#endif
// char buffer[250];
// snprintf(buffer, sizeof(buffer), "Zone -- %s\n\n", sourceFilePath_.c_str());
// ShowConsoleMsg(buffer);
