#ifndef CSURF_FADERPORT_UI_FUNCTIONS_H_
#define CSURF_FADERPORT_UI_FUNCTIONS_H_

#include <wdltypes.h>
#include <win32_utf8.h>
#include <localize.h>
#include <string>
#include <WDL/swell/swell-types.h>
#include <reaper_plugin_functions.h>
#include "../src/resource.h"
#include "extern/ini.hpp"
#include "csurf_utils.hpp"
#include <localize/localize.h>
#include "csurf_faderport_ui_utils.hpp"
#include <swell/swell-functions.h>

extern HWND g_hwnd;

using namespace std;

namespace CSURF_FADERPORT_UI_FUNCTIONS
{
    static HWND s_hwndReaSonusFunctionsDlg = NULL;

    /**
     * FUNCTIONS DIALOG
     */
    static WDL_DLGRET dlgProcFunctions(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        (void)lParam;
        mINI::INIFile file(GetReaSonusIniPath());
        mINI::INIStructure ini;
        file.read(ini);

        switch (uMsg)
        {
        case WM_INITDIALOG:
        {
            SetDlgItemText(hwndDlg, IDC_EDIT_Function1, const_cast<char *>(ini["Functions"]["1"].c_str()));
            SetDlgItemText(hwndDlg, IDC_EDIT_Function2, const_cast<char *>(ini["Functions"]["2"].c_str()));
            SetDlgItemText(hwndDlg, IDC_EDIT_Function3, const_cast<char *>(ini["Functions"]["3"].c_str()));
            SetDlgItemText(hwndDlg, IDC_EDIT_Function4, const_cast<char *>(ini["Functions"]["4"].c_str()));
            SetDlgItemText(hwndDlg, IDC_EDIT_Function5, const_cast<char *>(ini["Functions"]["5"].c_str()));
            SetDlgItemText(hwndDlg, IDC_EDIT_Function6, const_cast<char *>(ini["Functions"]["6"].c_str()));
            SetDlgItemText(hwndDlg, IDC_EDIT_Function7, const_cast<char *>(ini["Functions"]["7"].c_str()));
            SetDlgItemText(hwndDlg, IDC_EDIT_Function8, const_cast<char *>(ini["Functions"]["8"].c_str()));

            break;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            // Handle save click
            case IDOK:
                char buffer[255];
                GetDlgItemText(hwndDlg, IDC_EDIT_Function1, buffer, size(buffer));
                ini["Functions"]["1"] = buffer;
                GetDlgItemText(hwndDlg, IDC_EDIT_Function2, buffer, size(buffer));
                ini["Functions"]["2"] = buffer;
                GetDlgItemText(hwndDlg, IDC_EDIT_Function3, buffer, size(buffer));
                ini["Functions"]["3"] = buffer;
                GetDlgItemText(hwndDlg, IDC_EDIT_Function4, buffer, size(buffer));
                ini["Functions"]["4"] = buffer;
                GetDlgItemText(hwndDlg, IDC_EDIT_Function5, buffer, size(buffer));
                ini["Functions"]["5"] = buffer;
                GetDlgItemText(hwndDlg, IDC_EDIT_Function6, buffer, size(buffer));
                ini["Functions"]["6"] = buffer;
                GetDlgItemText(hwndDlg, IDC_EDIT_Function7, buffer, size(buffer));
                ini["Functions"]["7"] = buffer;
                GetDlgItemText(hwndDlg, IDC_EDIT_Function8, buffer, size(buffer));
                ini["Functions"]["8"] = buffer;

                file.write(ini);
                ShowWindow(s_hwndReaSonusFunctionsDlg, SW_HIDE);
                break;

            // Handle Cancel click
            case IDCANCEL:
                ShowWindow(s_hwndReaSonusFunctionsDlg, SW_HIDE);
                break;

            case IDC_BUTTON_Action1:
                ShowActionList(0, NULL);
                break;
            }
            break;
        case WM_CLOSE:
            ShowWindow(s_hwndReaSonusFunctionsDlg, SW_HIDE);
            break;
        }
        return 0;
    };

    static void ShowFunctionsDialog()
    {
        if (s_hwndReaSonusFunctionsDlg == NULL)
        {
            s_hwndReaSonusFunctionsDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_REASONUS_NATIVE_FUNCTIONS), g_hwnd, dlgProcFunctions);
        }
        ShowWindow(s_hwndReaSonusFunctionsDlg, SW_SHOW);
    }

    static void HideFunctionsDialog()
    {
        ShowWindow(s_hwndReaSonusFunctionsDlg, SW_HIDE);
        s_hwndReaSonusFunctionsDlg = NULL;
    }

    static bool IsFunctionsDialogOpen()
    {
        return s_hwndReaSonusFunctionsDlg != NULL;
    }
}

#endif