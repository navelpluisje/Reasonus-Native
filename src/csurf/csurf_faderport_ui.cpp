#include <wdltypes.h>
#include <win32_utf8.h>
#include <localize.h>
#include <string>
#include <WDL/swell/swell-types.h>
#include <WDL/swell/swell-functions.h>
#include <reaper_plugin_functions.h>
#include "../src/resource.h"
#include "extern/ini.hpp"
#include "csurf_utils.hpp"

using namespace std;

static void parseParms(const char *str, int parms[4])
{
    parms[0] = 0;
    parms[1] = 9;
    parms[2] = parms[3] = -1;

    const char *p = str;
    if (p)
    {
        int x = 0;
        while (x < 4)
        {
            while (*p == ' ')
                p++;
            if ((*p < '0' || *p > '9') && *p != '-')
                break;
            parms[x++] = atoi(p);
            while (*p && *p != ' ')
                p++;
        }
    }
}

static int AddComboEntry(HWND hwndDlg, int x, char *buf, int comboId)
{
    int a = (int)SendDlgItemMessage(hwndDlg, comboId, CB_ADDSTRING, 0, (LPARAM)buf);
    SendDlgItemMessage(hwndDlg, comboId, CB_SETITEMDATA, a, x);
    return a;
}

static WDL_DLGRET dlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    mINI::INIFile file(GetReaSonusIniPath());
    mINI::INIStructure ini;
    file.read(ini);

    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        int combo;
        char buf[255];
        char *noDeviceString = const_cast<char *>(__LOCALIZE("No device selected", "reasonus-faderport"));
        char *noSurfaceString = const_cast<char *>(__LOCALIZE("No surface selected", "reasonus-faderport"));

        WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_MidiIn));
        WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_MidiOut));
        WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_Surface));

        for (int i = 0; i <= GetNumMIDIInputs(); ++i)
        {
            if (i == 0)
            {
                combo = AddComboEntry(hwndDlg, 0, noDeviceString, IDC_COMBO_MidiIn);
                if (stoi(ini["Surface"]["MidiIn"]) == 0)
                {
                    SendDlgItemMessage(hwndDlg, IDC_COMBO_MidiIn, CB_SETCURSEL, combo, 0);
                }
            }
            if (GetMIDIInputName(i, buf, sizeof(buf)))
            {
                combo = AddComboEntry(hwndDlg, i, buf, IDC_COMBO_MidiIn);
                if (i == stoi(ini["Surface"]["MidiIn"]))
                {
                    SendDlgItemMessage(hwndDlg, IDC_COMBO_MidiIn, CB_SETCURSEL, combo, 0);
                }
            }
        }

        for (int i = 1; i <= GetNumMIDIOutputs(); ++i)
        {
            if (i == 0)
            {
                combo = AddComboEntry(hwndDlg, 0, noDeviceString, IDC_COMBO_MidiOut);
                if (stoi(ini["Surface"]["MidiOut"]) == 0)
                {
                    SendDlgItemMessage(hwndDlg, IDC_COMBO_MidiOut, CB_SETCURSEL, combo, 0);
                }
            }
            if (GetMIDIOutputName(i, buf, sizeof(buf)))
            {
                int dev = AddComboEntry(hwndDlg, i, buf, IDC_COMBO_MidiOut);
                if (i == stoi(ini["Surface"]["MidiOut"]))
                {
                    SendDlgItemMessage(hwndDlg, IDC_COMBO_MidiOut, CB_SETCURSEL, dev, 0);
                }
            }
        }

        combo = AddComboEntry(hwndDlg, 0, noSurfaceString, IDC_COMBO_Surface);
        if (ini["Surface"]["Surface"] == "0")
        {
            SendDlgItemMessage(hwndDlg, IDC_COMBO_Surface, CB_SETCURSEL, combo, 0);
        }
        combo = AddComboEntry(hwndDlg, 8, const_cast<char *>("Faderport 8"), IDC_COMBO_Surface);
        if (ini["Surface"]["Surface"] == "8")
        {
            SendDlgItemMessage(hwndDlg, IDC_COMBO_Surface, CB_SETCURSEL, combo, 0);
        }
        combo = AddComboEntry(hwndDlg, 16, const_cast<char *>("Faderport 16"), IDC_COMBO_Surface);
        if (ini["Surface"]["Surface"] == "16")
        {
            SendDlgItemMessage(hwndDlg, IDC_COMBO_Surface, CB_SETCURSEL, combo, 0);
        }
        break;
    }
    case WM_USER + 1024:
    {

        if (wParam > 1 && lParam)
        {
            int indev = -1, outdev = -1, surface = -1;

            int r = SendDlgItemMessage(hwndDlg, IDC_COMBO_MidiIn, CB_GETCURSEL, 0, 0);
            if (r != CB_ERR)
                indev = SendDlgItemMessage(hwndDlg, IDC_COMBO_MidiIn, CB_GETITEMDATA, r, 0);

            r = SendDlgItemMessage(hwndDlg, IDC_COMBO_MidiOut, CB_GETCURSEL, 0, 0);
            if (r != CB_ERR)
                outdev = SendDlgItemMessage(hwndDlg, IDC_COMBO_MidiOut, CB_GETITEMDATA, r, 0);

            r = SendDlgItemMessage(hwndDlg, IDC_COMBO_Surface, CB_GETCURSEL, 0, 0);
            if (r != CB_ERR)
                surface = SendDlgItemMessage(hwndDlg, IDC_COMBO_Surface, CB_GETITEMDATA, r, 0);

            ini["Surface"]["MidiIn"] = to_string(indev);
            ini["Surface"]["MidiOut"] = to_string(outdev);
            ini["Surface"]["Surface"] = to_string(surface);
            file.write(ini);
        }
        break;
    }
    }
    return 0;
};
