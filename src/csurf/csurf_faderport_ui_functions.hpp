#ifndef CSURF_FADERPORT_UI_FUNCTIONS_H_
#define CSURF_FADERPORT_UI_FUNCTIONS_H_

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

using namespace std;

namespace CSURF_FADERPORT_UI_FUNCTIONS
{
    static mINI::INIStructure ini;
    static HWND s_hwndReaSonusFunctionsDlg = NULL;
    static string functionsDlgSelectedFunction = "";
    static bool g_querying_action = false;
    static HWND s_hwndDlg;

    static void PopulateActionFields(HWND hwndDlg)
    {
        SetDlgItemText(hwndDlg, IDC_EDIT_FUNCTION_1, const_cast<char *>(ini["Functions"]["1"].c_str()));
        SetDlgItemText(hwndDlg, IDC_EDIT_FUNCTION_2, const_cast<char *>(ini["Functions"]["2"].c_str()));
        SetDlgItemText(hwndDlg, IDC_EDIT_FUNCTION_3, const_cast<char *>(ini["Functions"]["3"].c_str()));
        SetDlgItemText(hwndDlg, IDC_EDIT_FUNCTION_4, const_cast<char *>(ini["Functions"]["4"].c_str()));
        SetDlgItemText(hwndDlg, IDC_EDIT_FUNCTION_5, const_cast<char *>(ini["Functions"]["5"].c_str()));
        SetDlgItemText(hwndDlg, IDC_EDIT_FUNCTION_6, const_cast<char *>(ini["Functions"]["6"].c_str()));
        SetDlgItemText(hwndDlg, IDC_EDIT_FUNCTION_7, const_cast<char *>(ini["Functions"]["7"].c_str()));
        SetDlgItemText(hwndDlg, IDC_EDIT_FUNCTION_8, const_cast<char *>(ini["Functions"]["8"].c_str()));
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

    static void ShowFunctionInfo(HWND hwndDlg, string index)
    {
        int actionId = stoi(ini["Functions"][index]);
        const char *fullName = kbd_getTextFromCmd(actionId, 0);
        vector<string> actionInfo = split(fullName, ":");

        SetDlgItemText(hwndDlg, IDC_GROUP_FUNCTION_INFO, const_cast<char *>(("Function Info " + index).c_str()));
        SetDlgItemText(hwndDlg, IDC_FUNCTION_INFO_ID, const_cast<char *>(ini["Functions"][index].c_str()));
        SetDlgItemText(hwndDlg, IDC_FUNCTION_INFO_TYPE, const_cast<char *>(actionInfo[0].c_str()));
        SetDlgItemText(hwndDlg, IDC_FUNCTION_INFO_NAME, const_cast<char *>(actionInfo[1].c_str()));
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
            case IDCANCEL:
                HideFunctionsDialog();
                break;

            case IDC_BUTTON_ACTION_1:
                PromptForFunctionAction(hwndDlg, "1");
                break;

            case IDC_BUTTON_ACTION_2:
                PromptForFunctionAction(hwndDlg, "2");
                break;

            case IDC_BUTTON_ACTION_3:
                PromptForFunctionAction(hwndDlg, "3");
                break;

            case IDC_BUTTON_ACTION_4:
                PromptForFunctionAction(hwndDlg, "4");
                break;

            case IDC_BUTTON_ACTION_5:
                PromptForFunctionAction(hwndDlg, "5");
                break;

            case IDC_BUTTON_ACTION_6:
                PromptForFunctionAction(hwndDlg, "6");
                break;

            case IDC_BUTTON_ACTION_7:
                PromptForFunctionAction(hwndDlg, "7");
                break;

            case IDC_BUTTON_ACTION_8:
                PromptForFunctionAction(hwndDlg, "8");
                break;

            case IDC_BUTTON_INFO_1:
                ShowFunctionInfo(hwndDlg, "1");
                break;

            case IDC_BUTTON_INFO_2:
                ShowFunctionInfo(hwndDlg, "2");
                break;

            case IDC_BUTTON_INFO_3:
                ShowFunctionInfo(hwndDlg, "3");
                break;

            case IDC_BUTTON_INFO_4:
                ShowFunctionInfo(hwndDlg, "4");
                break;

            case IDC_BUTTON_INFO_5:
                ShowFunctionInfo(hwndDlg, "5");
                break;

            case IDC_BUTTON_INFO_6:
                ShowFunctionInfo(hwndDlg, "6");
                break;

            case IDC_BUTTON_INFO_7:
                ShowFunctionInfo(hwndDlg, "7");
                break;

            case IDC_BUTTON_INFO_8:
                ShowFunctionInfo(hwndDlg, "8");
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