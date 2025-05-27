#ifndef CSURF_FP_8_UI_INIT_H_
#define CSURF_FP_8_UI_INIT_H_

#include <string>
#include <mini/ini.h>
#include <WDL/win32_utf8.h>
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_faderport_ui_utils.hpp"
#include "../resource.h"

extern HWND g_hwnd;
extern REAPER_PLUGIN_HINSTANCE g_hInst;

namespace CSURF_FP_8_UI_INIT
{
    mINI::INIStructure ini;

    static void SaveCheckBoxValue(HWND hwndDlg, std::string key, int checkBox)
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_8));

        ini["surface"][key] = std::to_string(IsDlgButtonChecked(hwndDlg, checkBox));
        file.write(ini, true);
    }

    static WDL_DLGRET dlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {

        switch (uMsg)
        {
        case WM_INITDIALOG:
        {
            mINI::INIFile file(GetReaSonusIniPath(FP_8));
            readAndCreateIni(ini, FP_8);

            int combo;
            char buf[255];
            std::string noDeviceString = "No device selected";
            std::string noSurfaceString = "No surface selected";

            WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_MIDI_IN));
            WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_MIDI_OUT));
            WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_SURFACE));

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

            combo = AddComboEntry(hwndDlg, 0, const_cast<char *>(noSurfaceString.c_str()), IDC_COMBO_SURFACE);
            if (ini["surface"]["surface"] == "0")
            {
                SendDlgItemMessage(hwndDlg, IDC_COMBO_SURFACE, CB_SETCURSEL, combo, 0);
            }
            combo = AddComboEntry(hwndDlg, 8, const_cast<char *>("Faderport 8"), IDC_COMBO_SURFACE);
            if (ini["surface"]["surface"] == "8")
            {
                SendDlgItemMessage(hwndDlg, IDC_COMBO_SURFACE, CB_SETCURSEL, combo, 0);
            }
            combo = AddComboEntry(hwndDlg, 16, const_cast<char *>("Faderport 16"), IDC_COMBO_SURFACE);
            if (ini["surface"]["surface"] == "16")
            {
                SendDlgItemMessage(hwndDlg, IDC_COMBO_SURFACE, CB_SETCURSEL, combo, 0);
            }

            for (int i = 0; i < 6; i++)
            {
                int dev = AddComboEntry(hwndDlg, time_code_indexes[i], const_cast<char *>(time_code_names[i].c_str()), IDC_COMBO_INIT_TIME_CODE);
                if (time_code_indexes[i] == stoi(ini["surface"]["time-code"]))
                {
                    SendDlgItemMessage(hwndDlg, IDC_COMBO_INIT_TIME_CODE, CB_SETCURSEL, dev, 0);
                }
            }

            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_INIT_DIS_PLUGIN), BM_SETCHECK, ini["surface"]["disable-plugins"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_INIT_SWAP_SHIFT), BM_SETCHECK, ini["surface"]["swap-shift-buttons"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_INIT_MUTE_MOMENTARY), BM_SETCHECK, ini["surface"]["mute-solo-momentary"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_INIT_TIME_CODE), BM_SETCHECK, ini["surface"]["overwrite-time-code"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);

            break;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDC_BUTTON_DOCUMENTTION:
            {
                SystemOpenURL("https://navelpluisje.github.io/reasonus/documentation/faderport8/");
                break;
            }

            case IDC_CHECK_INIT_DIS_PLUGIN:
            {
                SaveCheckBoxValue(hwndDlg, "disable-plugins", IDC_CHECK_INIT_DIS_PLUGIN);
                break;
            }

            case IDC_CHECK_INIT_SWAP_SHIFT:
            {
                SaveCheckBoxValue(hwndDlg, "swap-shift-buttons", IDC_CHECK_INIT_SWAP_SHIFT);
                break;
            }
            case IDC_CHECK_INIT_MUTE_MOMENTARY:
            {
                SaveCheckBoxValue(hwndDlg, "mute-solo-momentary", IDC_CHECK_INIT_MUTE_MOMENTARY);
                break;
            }
            case IDC_CHECK_INIT_TIME_CODE:
            {
                SaveCheckBoxValue(hwndDlg, "overwrite-time-code", IDC_CHECK_INIT_TIME_CODE);
                break;
            }
            break;
            }

        case WM_USER + 1024:
        {

            if (wParam > 1 && lParam)
            {
                static mINI::INIFile file(GetReaSonusIniPath(FP_8));

                LRESULT indev = -1, outdev = -1, surface = -1, time_code = -1;

                int r = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_GETCURSEL, 0, 0);
                if (r != CB_ERR)
                    indev = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_GETITEMDATA, r, 0);

                r = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_GETCURSEL, 0, 0);
                if (r != CB_ERR)
                    outdev = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_GETITEMDATA, r, 0);

                r = SendDlgItemMessage(hwndDlg, IDC_COMBO_SURFACE, CB_GETCURSEL, 0, 0);
                if (r != CB_ERR)
                    surface = SendDlgItemMessage(hwndDlg, IDC_COMBO_SURFACE, CB_GETITEMDATA, r, 0);

                r = SendDlgItemMessage(hwndDlg, IDC_COMBO_INIT_TIME_CODE, CB_GETCURSEL, 0, 0);
                if (r != CB_ERR)
                    time_code = SendDlgItemMessage(hwndDlg, IDC_COMBO_INIT_TIME_CODE, CB_GETITEMDATA, r, 0);

                ini["surface"]["midiin"] = std::to_string(indev);
                ini["surface"]["midiout"] = std::to_string(outdev);
                ini["surface"]["surface"] = std::to_string(surface);
                ini["surface"]["time-code"] = std::to_string(time_code);
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
        return CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_REASONUS_NATIVE), parent, CSURF_FP_8_UI_INIT::dlgProc, (LPARAM)initConfigString);
    }
}
#endif