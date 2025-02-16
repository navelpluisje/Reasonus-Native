#ifndef CSURF_FADERPORT_UI_H_
#define CSURF_FADERPORT_UI_H_

#include <string>
#include <mini/ini.h>
#include <WDL/win32_utf8.h>
#include "csurf_utils.hpp"
#include "csurf_faderport_ui_utils.hpp"
#include "../resource.h"

extern HWND g_hwnd;

namespace CSURF_FADERPORT_UI_INIT
{
    mINI::INIStructure ini;

    static void SaveCheckBoxValue(HWND hwndDlg, std::string key, int checkBox)
    {
        mINI::INIFile file(GetReaSonusIniPath());

        ini["surface"][key] = std::to_string(IsDlgButtonChecked(hwndDlg, checkBox));
        file.write(ini, true);
    }

    static WDL_DLGRET dlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {

        switch (uMsg)
        {
        case WM_INITDIALOG:
        {
            mINI::INIFile file(GetReaSonusIniPath());
            readAndCreateIni(ini);

            int combo;
            char buf[255];
            char *noDeviceString = "No device selected";
            char *noSurfaceString = "No surface selected";

            WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_MIDI_IN));
            WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_MIDI_OUT));
            WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_SURFACE));

            for (int i = 0; i <= GetNumMIDIInputs(); ++i)
            {
                if (i == 0)
                {
                    combo = AddComboEntry(hwndDlg, 0, noDeviceString, IDC_COMBO_MIDI_IN);
                    if (stoi(ini["Surface"]["MidiIn"]) == 0)
                    {
                        SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_SETCURSEL, combo, 0);
                    }
                }
                if (GetMIDIInputName(i, buf, sizeof(buf)))
                {
                    combo = AddComboEntry(hwndDlg, i, buf, IDC_COMBO_MIDI_IN);
                    if (i == stoi(ini["Surface"]["MidiIn"]))
                    {
                        SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_SETCURSEL, combo, 0);
                    }
                }
            }

            for (int i = 0; i <= GetNumMIDIOutputs(); ++i)
            {
                if (i == 0)
                {
                    combo = AddComboEntry(hwndDlg, 0, noDeviceString, IDC_COMBO_MIDI_OUT);
                    if (stoi(ini["Surface"]["MidiOut"]) == 0)
                    {
                        SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_SETCURSEL, combo, 0);
                    }
                }
                if (GetMIDIOutputName(i, buf, sizeof(buf)))
                {
                    int dev = AddComboEntry(hwndDlg, i, buf, IDC_COMBO_MIDI_OUT);
                    if (i == stoi(ini["Surface"]["MidiOut"]))
                    {
                        SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_SETCURSEL, dev, 0);
                    }
                }
            }

            combo = AddComboEntry(hwndDlg, 0, noSurfaceString, IDC_COMBO_SURFACE);
            if (ini["Surface"]["Surface"] == "0")
            {
                SendDlgItemMessage(hwndDlg, IDC_COMBO_SURFACE, CB_SETCURSEL, combo, 0);
            }
            combo = AddComboEntry(hwndDlg, 8, const_cast<char *>("Faderport 8"), IDC_COMBO_SURFACE);
            if (ini["Surface"]["Surface"] == "8")
            {
                SendDlgItemMessage(hwndDlg, IDC_COMBO_SURFACE, CB_SETCURSEL, combo, 0);
            }
            combo = AddComboEntry(hwndDlg, 16, const_cast<char *>("Faderport 16"), IDC_COMBO_SURFACE);
            if (ini["Surface"]["Surface"] == "16")
            {
                SendDlgItemMessage(hwndDlg, IDC_COMBO_SURFACE, CB_SETCURSEL, combo, 0);
            }
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_INIT_DIS_PLUGIN), BM_SETCHECK, ini["Surface"]["disable-plugins"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_INIT_SWAP_SHIFT), BM_SETCHECK, ini["Surface"]["swap-shift-buttons"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_INIT_MUTE_MOMENTARY), BM_SETCHECK, ini["Surface"]["mute-solo-momentary"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);

            break;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDC_BUTTON_DOCUMENTTION:
            {
                SystemOpenURL("https://navelpluisje.nl");
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
            break;
        }

        case WM_USER + 1024:
        {

            if (wParam > 1 && lParam)
            {
                static mINI::INIFile file(GetReaSonusIniPath());

                int indev = -1, outdev = -1, surface = -1;

                int r = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_GETCURSEL, 0, 0);
                if (r != CB_ERR)
                    indev = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_GETITEMDATA, r, 0);

                r = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_GETCURSEL, 0, 0);
                if (r != CB_ERR)
                    outdev = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_GETITEMDATA, r, 0);

                r = SendDlgItemMessage(hwndDlg, IDC_COMBO_SURFACE, CB_GETCURSEL, 0, 0);
                if (r != CB_ERR)
                    surface = SendDlgItemMessage(hwndDlg, IDC_COMBO_SURFACE, CB_GETITEMDATA, r, 0);

                ini["Surface"]["MidiIn"] = std::to_string(indev);
                ini["Surface"]["MidiOut"] = std::to_string(outdev);
                ini["Surface"]["Surface"] = std::to_string(surface);
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
        return CreateDialogParam(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_REASONUS_NATIVE), parent, dlgProc, (LPARAM)initConfigString);
    }
}
#endif