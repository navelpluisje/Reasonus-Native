#ifndef CSURF_FADERPORT_V2_UI_INIT_V2_H_
#define CSURF_FADERPORT_V2_UI_INIT_V2_H_

#include <string>
#include <mini/ini.h>
#include <WDL/win32_utf8.h>
#include <config.h>
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_faderport_ui_utils.hpp"
#include "../resource.h"

extern HWND g_hwnd;
extern REAPER_PLUGIN_HINSTANCE g_hInst;

namespace CSURF_FP_V2_UI_INIT
{
    mINI::INIStructure ini;

    static void SaveCheckBoxValue(HWND hwndDlg, std::string key, int checkBox)
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_V2));

        ini["surface"][key] = std::to_string(IsDlgButtonChecked(hwndDlg, checkBox));
        file.write(ini, true);
    }

    static WDL_DLGRET dlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {

        switch (uMsg)
        {
        case WM_INITDIALOG:
        {
            mINI::INIFile file(GetReaSonusIniPath(FP_V2));
            readAndCreateIni(ini, FP_V2);

            int combo;
            char buf[255];
            std::string noDeviceString = "No device selected";

            WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_MIDI_IN));
            WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_MIDI_OUT));

            for (int i = 0; i <= GetNumMIDIInputs(); ++i)
            {
                if (i == 0)
                {
                    combo = AddComboEntry(hwndDlg, 0, const_cast<char *>(noDeviceString.c_str()), IDC_COMBO_MIDI_IN);
                    if (stoi(ini["surface"]["midiin"]) == 0)
                    {
                        SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_SETCURSEL, combo, 0);
                    }
                }
                if (GetMIDIInputName(i, buf, sizeof(buf)))
                {
                    combo = AddComboEntry(hwndDlg, i, buf, IDC_COMBO_MIDI_IN);
                    if (i == stoi(ini["surface"]["midiin"]))
                    {
                        SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_SETCURSEL, combo, 0);
                    }
                }
            }

            for (int i = 0; i <= GetNumMIDIOutputs(); ++i)
            {
                if (i == 0)
                {
                    combo = AddComboEntry(hwndDlg, 0, const_cast<char *>(noDeviceString.c_str()), IDC_COMBO_MIDI_OUT);
                    if (stoi(ini["surface"]["midiout"]) == 0)
                    {
                        SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_SETCURSEL, combo, 0);
                    }
                }
                if (GetMIDIOutputName(i, buf, sizeof(buf)))
                {
                    int dev = AddComboEntry(hwndDlg, i, buf, IDC_COMBO_MIDI_OUT);
                    if (i == stoi(ini["surface"]["midiout"]))
                    {
                        SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_SETCURSEL, dev, 0);
                    }
                }
            }

            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_INIT_MUTE_MOMENTARY), BM_SETCHECK, ini["surface"]["mute-solo-momentary"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_CONTROL_HIDDEN_TRACKS), BM_SETCHECK, ini["surface"]["control-hidden-tracks"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);

            SetDlgItemText(hwndDlg, IDC_VERSION, GIT_VERSION);

            break;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDC_BUTTON_DOCUMENTTION:
            {
                SystemOpenURL("https://navelpluisje.github.io/reasonus/documentation/faderportv2/");
                break;
            }

            case IDC_CHECK_INIT_MUTE_MOMENTARY:
            {
                SaveCheckBoxValue(hwndDlg, "mute-solo-momentary", IDC_CHECK_INIT_MUTE_MOMENTARY);
                break;
            }

            case IDC_CHECK_CONTROL_HIDDEN_TRACKS:
            {
                SaveCheckBoxValue(hwndDlg, "control-hidden-tracks", IDC_CHECK_CONTROL_HIDDEN_TRACKS);
                break;
            }
            break;
            }

        case WM_USER + 1024:
        {

            if (wParam > 1 && lParam)
            {
                static mINI::INIFile file(GetReaSonusIniPath(FP_V2));

                LRESULT indev = -1, outdev = -1;

                int r = (int)SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_GETCURSEL, 0, 0);
                if (r != CB_ERR)
                    indev = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_GETITEMDATA, r, 0);

                r = (int)SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_GETCURSEL, 0, 0);
                if (r != CB_ERR)
                    outdev = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_GETITEMDATA, r, 0);

                ini["surface"]["midiin"] = std::to_string(indev);
                ini["surface"]["midiout"] = std::to_string(outdev);
                file.write(ini, true);
            }
            break;
        }
        }
        return 0;
    };

    static HWND CreateInitDialog(const char *type_string, HWND parent, const char *initConfigString)
    {
        (void)type_string;
        return CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_REASONUS_NATIVE_V2), parent, CSURF_FP_V2_UI_INIT::dlgProc, (LPARAM)initConfigString);
    }
}
#endif