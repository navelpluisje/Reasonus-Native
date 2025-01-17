#ifndef CSURF_FADERPORT_UI_PLUGIN_EDIT_H_
#define CSURF_FADERPORT_UI_PLUGIN_EDIT_H_

#include <wdltypes.h>
#include <win32_utf8.h>
#include <string>
#include <WDL/swell/swell-types.h>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include "../resource.h"
#include <mini/ini.h>
#include "csurf_utils.hpp"
#include "csurf_faderport_ui_utils.hpp"
#include <localize/localize.h>
#include <swell/swell-functions.h>

extern HWND g_hwnd;

using namespace std;

enum FilterListDirection
{
    FilterListUp = -1,
    FilterListDown = 1,
};

namespace CSURF_FADERPORT_UI_PLUGIN_EDIT
{
    static mINI::INIStructure ini;
    static HWND s_hwndReaSonusPluginEditDlg = NULL;
    static string fileName;
    static string controlIndex;

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

    static void StoreField(string controlName, string fieldName, string value)
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
                GetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_SELECT_NAME, buffer, size(buffer));
                StoreField("Select_" + controlIndex, "name", string(buffer));
                break;
            }

            case IDC_EDIT_PLUGIN_EDIT_SELECT_PARAM_ID:
            {
                char buffer[255];
                GetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_SELECT_PARAM_ID, buffer, size(buffer));
                StoreField("Select_" + controlIndex, "param", string(buffer));
                break;
            }

            case IDC_EDIT_PLUGIN_EDIT_SELECT_NB_STEPS:
            {
                char buffer[255];
                GetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_SELECT_NB_STEPS, buffer, size(buffer));
                StoreField("Select_" + controlIndex, "steps", string(buffer));
                break;
            }

            case IDC_EDIT_PLUGIN_EDIT_FADER_NAME:
            {
                char buffer[255];
                GetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_FADER_NAME, buffer, size(buffer));
                StoreField("Fader_" + controlIndex, "name", string(buffer));
                break;
            }

            case IDC_EDIT_PLUGIN_EDIT_FADER_PARAM_ID:
            {
                char buffer[255];
                GetDlgItemText(hwndDlg, IDC_EDIT_PLUGIN_EDIT_FADER_PARAM_ID, buffer, size(buffer));
                StoreField("Fader_" + controlIndex, "param", string(buffer));
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

    static void ShowPluginEditDialog(string _fileName, int index)
    {
        fileName = _fileName;
        controlIndex = to_string(index);

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
