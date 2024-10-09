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
    // static mINI::INIFile file(GetReaSonusIniPath());
    static mINI::INIStructure ini;
    static HWND s_hwndReaSonusFunctionsDlg = NULL;
    static string functionsDlgSelectedFunction = "";
    static bool g_querying_action = false;
    static HWND s_hwndDlg;

    static void PopulateActionFields(HWND hwndDlg)
    {
        // Add the description to it
        SetDlgItemText(hwndDlg, IDC_EDIT_Function1, const_cast<char *>(ini["Functions"]["1"].c_str()));
        SetDlgItemText(hwndDlg, IDC_EDIT_Function2, const_cast<char *>(ini["Functions"]["2"].c_str()));
        SetDlgItemText(hwndDlg, IDC_EDIT_Function3, const_cast<char *>(ini["Functions"]["3"].c_str()));
        SetDlgItemText(hwndDlg, IDC_EDIT_Function4, const_cast<char *>(ini["Functions"]["4"].c_str()));
        SetDlgItemText(hwndDlg, IDC_EDIT_Function5, const_cast<char *>(ini["Functions"]["5"].c_str()));
        SetDlgItemText(hwndDlg, IDC_EDIT_Function6, const_cast<char *>(ini["Functions"]["6"].c_str()));
        SetDlgItemText(hwndDlg, IDC_EDIT_Function7, const_cast<char *>(ini["Functions"]["7"].c_str()));
        SetDlgItemText(hwndDlg, IDC_EDIT_Function8, const_cast<char *>(ini["Functions"]["8"].c_str()));
    }

    static void mytimer()
    {
        WDL_ASSERT(g_querying_action == true);
        const int actionId = PromptForAction(0, 0, 0);
        if (actionId == 0)
        {
            return;
        }
        plugin_register("-timer", (void *)&mytimer);
        g_querying_action = false;
        if (actionId > 0)
        {
            static mINI::INIFile file(GetReaSonusIniPath());
            ini["Functions"][functionsDlgSelectedFunction] = to_string(actionId);
            file.write(ini);
            PopulateActionFields(s_hwndDlg);
        }
        PromptForAction(-1, 0, 0);
    }

    static void PromptForFunctionAction(HWND hwndDlg, string index)
    {
        functionsDlgSelectedFunction = index;
        s_hwndDlg = hwndDlg;
        PromptForAction(1, 0, 0);

        if (!g_querying_action)
        {
            g_querying_action = true;
            plugin_register("timer", (void *)&mytimer);
        }
    }

    static void HideFunctionsDialog()
    {
        ShowWindow(s_hwndReaSonusFunctionsDlg, SW_HIDE);
        s_hwndReaSonusFunctionsDlg = NULL;
    }

    /**
     * FUNCTIONS DIALOG
     */
    static WDL_DLGRET dlgProcFunctions(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        (void)lParam;

        switch (uMsg)
        {
        case WM_INITDIALOG:
        {
            PopulateActionFields(hwndDlg);
            break;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            // This can probably be removed
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

                static mINI::INIFile file(GetReaSonusIniPath());
                file.write(ini);
                HideFunctionsDialog();
                break;

            // Handle Cancel click
            case IDCANCEL:
                HideFunctionsDialog();
                break;

            case IDC_BUTTON_Action1:
                PromptForFunctionAction(hwndDlg, "1");
                break;

            case IDC_BUTTON_Action2:
                PromptForFunctionAction(hwndDlg, "2");
                break;

            case IDC_BUTTON_Action3:
                PromptForFunctionAction(hwndDlg, "3");
                break;

            case IDC_BUTTON_Action4:
                PromptForFunctionAction(hwndDlg, "4");
                break;

            case IDC_BUTTON_Action5:
                PromptForFunctionAction(hwndDlg, "5");
                break;

            case IDC_BUTTON_Action6:
                PromptForFunctionAction(hwndDlg, "6");
                break;

            case IDC_BUTTON_Action7:
                PromptForFunctionAction(hwndDlg, "7");
                break;

            case IDC_BUTTON_Action8:
                PromptForFunctionAction(hwndDlg, "8");
                break;
            }
            break;
        case WM_CLOSE:
            HideFunctionsDialog();
            break;
        }
        return 0;
    };

    static void ShowFunctionsDialog()
    {
        static mINI::INIFile file(GetReaSonusIniPath());
        file.read(ini);

        if (s_hwndReaSonusFunctionsDlg == NULL)
        {
            s_hwndReaSonusFunctionsDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_REASONUS_NATIVE_FUNCTIONS), g_hwnd, dlgProcFunctions);
        }
        ShowWindow(s_hwndReaSonusFunctionsDlg, SW_SHOW);
    }

    static bool IsFunctionsDialogOpen()
    {
        return s_hwndReaSonusFunctionsDlg != NULL;
    }
}

#endif