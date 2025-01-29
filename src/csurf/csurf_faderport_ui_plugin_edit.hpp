#ifndef CSURF_FADERPORT_UI_PLUGIN_EDIT_H_
#define CSURF_FADERPORT_UI_PLUGIN_EDIT_H_

#include <string>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <WDL/win32_utf8.h>
#include <WDL/localize/localize.h>
#include <reaper_plugin_functions.h>
#include <mini/ini.h>
#include "../resource.h"
#include "csurf_utils.hpp"
#include "csurf_faderport_ui_utils.hpp"

extern HWND g_hwnd;

enum FilterListDirection
{
    FilterListUp = -1,
    FilterListDown = 1,
};

namespace CSURF_FADERPORT_UI_PLUGIN_EDIT
{
    static mINI::INIStructure ini;
    static HWND s_hwndReaSonusPluginEditDlg = NULL;
    static std::string fileName;
    static std::string controlIndex;

    static void HidePluginEditDialog()
    {
        ShowWindow(s_hwndReaSonusPluginEditDlg, SW_HIDE);
        s_hwndReaSonusPluginEditDlg = NULL;
    }

    static void PopulateFields(HWND hwndDlg)
    {
        if (ini.has("Select_" + controlIndex))
        {
            SetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_SELECT_NAME, ini["Select_" + controlIndex]["name"].c_str());
            SetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_ORIG_NAME, ini["Select_" + controlIndex]["origName"].c_str());
            SetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_SELECT_PARAM_ID, ini["Select_" + controlIndex]["param"].c_str());
            SetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_SELECT_NB_STEPS, ini["Select_" + controlIndex]["steps"].c_str());
        }
        else
        {
            SetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_SELECT_NAME, "");
            SetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_ORIG_NAME, "");
            SetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_SELECT_PARAM_ID, "");
            SetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_SELECT_NB_STEPS, "");
        }
        if (ini.has("Fader_" + controlIndex))
        {
            SetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_FADER_NAME, ini["Fader_" + controlIndex]["name"].c_str());
            SetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_FADER_ORIG_NAME, ini["Fader_" + controlIndex]["origName"].c_str());
            SetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_FADER_PARAM_ID, ini["Fader_" + controlIndex]["param"].c_str());
        }
        else
        {
            SetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_FADER_NAME, "");
            SetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_FADER_ORIG_NAME, "");
            SetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_FADER_PARAM_ID, "");
        }
    }

    static void StoreField(std::string controlName, std::string fieldName, std::string value)
    {
        if (ini[controlName][fieldName] != value)
        {
            mINI::INIFile file(fileName);
            ini[controlName][fieldName] = value;
            file.write(ini);
        }
    }

    static WDL_DLGRET dlgProcFiters(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        (void)lParam;
        switch (uMsg)
        {
        case WM_INITDIALOG:
        {
            PopulateFields(hwndDlg);
            break;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {

            case IDC_LIST_FILTERS:
            {
                PopulateFields(hwndDlg);
                break;
            }

            case IDC_EDIT_PLUGIN_EDIT_SELECT_NAME:
            {
                char buffer[255];
                GetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_SELECT_NAME, buffer, std::size(buffer));
                StoreField("Select_" + controlIndex, "name", std::string(buffer));
                break;
            }

            case IDC_EDIT_PLUGIN_EDIT_SELECT_PARAM_ID:
            {
                char buffer[255];
                GetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_SELECT_PARAM_ID, buffer, std::size(buffer));
                StoreField("Select_" + controlIndex, "param", std::string(buffer));
                break;
            }

            case IDC_EDIT_PLUGIN_EDIT_SELECT_NB_STEPS:
            {
                char buffer[255];
                GetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_SELECT_NB_STEPS, buffer, std::size(buffer));
                StoreField("Select_" + controlIndex, "steps", std::string(buffer));
                break;
            }

            case IDC_EDIT_PLUGIN_EDIT_FADER_NAME:
            {
                char buffer[255];
                GetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_FADER_NAME, buffer, std::size(buffer));
                StoreField("Fader_" + controlIndex, "name", std::string(buffer));
                break;
            }

            case IDC_EDIT_PLUGIN_EDIT_FADER_PARAM_ID:
            {
                char buffer[255];
                GetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_FADER_PARAM_ID, buffer, std::size(buffer));
                StoreField("Fader_" + controlIndex, "param", std::string(buffer));
                break;
            }

            // Handle Cancel click
            case IDCANCEL:
                HidePluginEditDialog();
                break;
            }
            break;

        case WM_CLOSE:
            HidePluginEditDialog();
            break;
        }
        return 0;
    };

    static void ShowPluginEditDialog(std::string _fileName, int index)
    {
        fileName = _fileName;
        controlIndex = std::to_string(index);

        static mINI::INIFile file(fileName);
        file.read(ini);

        if (s_hwndReaSonusPluginEditDlg == NULL)
        {
            s_hwndReaSonusPluginEditDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_REASONUS_NATIVE_PLUGIN_EDIT), g_hwnd, dlgProcFiters);
        }
        ShowWindow(s_hwndReaSonusPluginEditDlg, SW_SHOW);
    }

    static bool IsPluginEditDialogOpen()
    {
        return s_hwndReaSonusPluginEditDlg != NULL;
    }
} // namespace

#endif
