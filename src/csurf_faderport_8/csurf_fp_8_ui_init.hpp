#ifndef CSURF_FP_8_UI_INIT_H_
#define CSURF_FP_8_UI_INIT_H_

#include <string>
#include <mini/ini.h>
#include <WDL/win32_utf8.h>
#include <config.h>
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_faderport_ui_utils.hpp"
#include "../resource.h"

extern HWND g_hwnd;
extern REAPER_PLUGIN_HINSTANCE g_hInst;

namespace CSURF_FP_8_UI_INIT {
    mINI::INIStructure ini;

    static void HandleMidiMessage(HWND hwndDlg, const int indev, const int outdev) {
        const bool indev_disabled = isMidiInDeviceDisabled(indev);
        const bool outdev_disabled = isMidiOutDeviceDisabled(outdev);

        if (
            (!indev_disabled && indev > -1)
            || (!outdev_disabled && outdev > -1)
        ) {
            if (!indev_disabled && outdev_disabled) {
                SetDlgItemText(hwndDlg, IDC_MIDI_DISABLED_1, "The MIDI In device is not disabled.");
            } else if (indev_disabled && !outdev_disabled) {
                SetDlgItemText(hwndDlg, IDC_MIDI_DISABLED_1, "The MIDI Out device is not disabled.");
            } else {
                SetDlgItemText(hwndDlg, IDC_MIDI_DISABLED_1, "The MIDI In and Out device is not disabled.");
            }

            SetDlgItemText(hwndDlg, IDC_MIDI_DISABLED_2, "This is needed to make ReaSonus function properly.");
        } else {
            SetDlgItemText(hwndDlg, IDC_MIDI_DISABLED_1, "");
            SetDlgItemText(hwndDlg, IDC_MIDI_DISABLED_2, "");
        }
    }

    static WDL_DLGRET dlgProc(HWND hwndDlg, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
        switch (uMsg) {
            case WM_INITDIALOG: {
                const mINI::INIFile file(GetReaSonusIniPath(FP_8));
                ReaSonusSettings::GetInstance(FP_8)->ReadAndCreateIni(ini);

                int combo;
                char buf[255]; // NOLINT(*-avoid-c-arrays)
                const std::string noDeviceString = "No device selected";
                const std::string noSurfaceString = "No surface selected";

                WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_MIDI_IN));
                WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_MIDI_OUT));
                WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_SURFACE));

                for (int i = 0; i <= GetNumMIDIInputs(); ++i) {
                    if (i == 0) {
                        combo = AddComboEntry(
                            hwndDlg,
                            0,
                            noDeviceString.c_str(),
                            IDC_COMBO_MIDI_IN
                        );

                        if (stoi(ini["surface"]["midiin"]) == 0) {
                            SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_SETCURSEL, combo, 0);
                        }
                    }

                    if (GetMIDIInputName(i, buf, sizeof(buf))) {
                        combo = AddComboEntry(hwndDlg, i, buf, IDC_COMBO_MIDI_IN);

                        if (i == stoi(ini["surface"]["midiin"])) {
                            SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_SETCURSEL, combo, 0);
                        }
                    }
                }

                for (int i = 0; i <= GetNumMIDIOutputs(); ++i) {
                    if (i == 0) {
                        combo = AddComboEntry(
                            hwndDlg,
                            0,
                            noDeviceString.c_str(),
                            IDC_COMBO_MIDI_OUT
                        );

                        if (stoi(ini["surface"]["midiout"]) == 0) {
                            SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_SETCURSEL, combo, 0);
                        }
                    }
                    if (GetMIDIOutputName(i, buf, sizeof(buf))) {
                        const int dev = AddComboEntry(hwndDlg, i, buf, IDC_COMBO_MIDI_OUT);

                        if (i == stoi(ini["surface"]["midiout"])) {
                            SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_SETCURSEL, dev, 0);
                        }
                    }
                }

                combo = AddComboEntry(hwndDlg, 0, noSurfaceString.c_str(), IDC_COMBO_SURFACE);

                if (ini["surface"]["surface"] == "0") {
                    SendDlgItemMessage(hwndDlg, IDC_COMBO_SURFACE, CB_SETCURSEL, combo, 0);
                }
                combo = AddComboEntry(hwndDlg, 8, const_cast<char *>("Faderport 8"), IDC_COMBO_SURFACE);
                if (ini["surface"]["surface"] == "8") {
                    SendDlgItemMessage(hwndDlg, IDC_COMBO_SURFACE, CB_SETCURSEL, combo, 0);
                }
                combo = AddComboEntry(hwndDlg, 16, const_cast<char *>("Faderport 16"), IDC_COMBO_SURFACE);
                if (ini["surface"]["surface"] == "16") {
                    SendDlgItemMessage(hwndDlg, IDC_COMBO_SURFACE, CB_SETCURSEL, combo, 0);
                }

                SetDlgItemText(hwndDlg, IDC_VERSION, GIT_VERSION);

                HandleMidiMessage(
                    hwndDlg,
                    stoi(ini["surface"]["midiin"]),
                    stoi(ini["surface"]["midiout"])
                );

                break;
            }

            case WM_COMMAND:
                switch (LOWORD(wParam)) {
                    case IDC_COMBO_MIDI_IN:
                    case IDC_COMBO_MIDI_OUT: {
                        LRESULT indev = -1;
                        LRESULT outdev = -1;

                        LRESULT combo_value = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_GETCURSEL, 0, 0);
                        if (combo_value != CB_ERR) {
                            indev = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_GETITEMDATA, combo_value, 0);
                        }

                        combo_value = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_GETCURSEL, 0, 0);
                        if (combo_value != CB_ERR) {
                            outdev = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_GETITEMDATA, combo_value, 0);
                        }

                        HandleMidiMessage(hwndDlg, indev, outdev);

                        break;
                    }

                    case IDC_BUTTON_DOCUMENTATION: {
                        SystemOpenURL("https://reasonus.net/documentation/faderport8/");
                        break;
                    }

                    case IDC_BUTTON_GITHUB: {
                        SystemOpenURL("https://github.com/navelpluisje/Reasonus-Native/issues");
                        break;
                    }

                    case IDC_BUTTON_REAPER: {
                        SystemOpenURL("https://forum.cockos.com/showthread.php?t=267116");
                        break;
                    }

                    case IDC_BUTTON_TIPEEE: {
                        SystemOpenURL("https://en.tipeee.com/navelpluisje");
                        break;
                    }

                    case IDC_BUTTON_COFFEE: {
                        SystemOpenURL("https://buymeacoffee.com/navelpluisje");
                        break;
                    }

                    default: ;
                }
                break;

            case WM_USER + 1024: {
                if (wParam > 1 && lParam != 0) {
                    const static mINI::INIFile file(GetReaSonusIniPath(FP_8));

                    LRESULT indev = -1;
                    LRESULT outdev = -1;
                    LRESULT surface = -1;

                    LRESULT combo_value = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_GETCURSEL, 0, 0);
                    if (combo_value != CB_ERR) {
                        indev = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_IN, CB_GETITEMDATA, combo_value, 0);
                    }

                    combo_value = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_GETCURSEL, 0, 0);
                    if (combo_value != CB_ERR) {
                        outdev = SendDlgItemMessage(hwndDlg, IDC_COMBO_MIDI_OUT, CB_GETITEMDATA, combo_value, 0);
                    }

                    combo_value = SendDlgItemMessage(hwndDlg, IDC_COMBO_SURFACE, CB_GETCURSEL, 0, 0);
                    if (combo_value != CB_ERR) {
                        surface = SendDlgItemMessage(hwndDlg, IDC_COMBO_SURFACE, CB_GETITEMDATA, combo_value, 0);
                    }

                    ini["surface"]["midiin"] = std::to_string(indev);
                    ini["surface"]["midiout"] = std::to_string(outdev);
                    ini["surface"]["surface"] = std::to_string(surface);
                    file.write(ini, true);
                }
                break;
            }
            default: ;
        }
        return 0;
    };

    static HWND CreateInitDialog(const char *type_string, HWND parent, const char *initConfigString) {
        (void) type_string;
        return CreateDialogParam(
            g_hInst,
            MAKEINTRESOURCE(IDD_DIALOG_REASONUS_NATIVE),
            parent,
            CSURF_FP_8_UI_INIT::dlgProc,
            reinterpret_cast<LPARAM>(initConfigString)
        );
    }
}
#endif
