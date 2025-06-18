#include "../shared/csurf_utils.hpp"
#include "csurf_fp_8_ui_functions.hpp"
#include <mini/ini.h>

extern HWND g_hwnd;
extern REAPER_PLUGIN_HINSTANCE g_hInst;

namespace CSURF_FP_UI_FUNCTIONS
{
    namespace
    {
        mINI::INIStructure ini;
        HWND s_hwndReaSonusFunctionsDlg = NULL;
        std::string functionsDlgSelectedFunction = "";
        bool g_querying_action = false;
        HWND s_hwndDlg;
        int positions[8][4] = {
            {30, 44, 374, 64},
            {30, 78, 374, 98},
            {30, 112, 374, 132},
            {30, 146, 374, 166},
            {30, 180, 374, 200},
            {30, 214, 374, 234},
            {30, 248, 374, 268},
            {30, 282, 374, 302},
        };
        int hoverIndex;
    }

    void PopulateActionFields(HWND hwndDlg)
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

    void mytimer()
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
            static mINI::INIFile file(GetReaSonusIniPath(FP_8));
            ini["Functions"][functionsDlgSelectedFunction] = std::to_string(actionId);
            file.write(ini, true);
            PopulateActionFields(s_hwndDlg);
        }

        PromptForAction(-1, 0, 0);
    }

    void PromptForFunctionAction(HWND hwndDlg, std::string index)
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

    void ShowFunctionInfo(HWND hwndDlg, std::string index)
    {
        int actionId = 0;

        if (stoi(index) > 0)
        {
            actionId = stoi(ini["Functions"][index]);
        }

        const char *fullName = kbd_getTextFromCmd(actionId, 0);
        std::vector<std::string> actionInfo = split(fullName, ":");

        SetDlgItemText(hwndDlg, IDC_GROUP_FUNCTION_INFO, ("Function Info " + index).c_str());
        SetDlgItemText(hwndDlg, IDC_FUNCTION_INFO_ID, ini["Functions"][index].c_str());
        SetDlgItemText(hwndDlg, IDC_FUNCTION_INFO_TYPE, actionInfo.size() > 0 ? actionInfo[0].c_str() : "");
        SetDlgItemText(hwndDlg, IDC_FUNCTION_INFO_NAME, actionInfo.size() > 1 ? actionInfo[1].c_str() : "");
    }

    void HideFunctionsDialog()
    {
        ShowWindow(s_hwndReaSonusFunctionsDlg, SW_HIDE);
        s_hwndReaSonusFunctionsDlg = NULL;
    }

    /**
     * FUNCTIONS DIALOG
     */
    WDL_DLGRET dlgProcFunctions(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        (void)lParam;

        switch (uMsg)
        {
        case WM_INITDIALOG:
        {
            PopulateActionFields(hwndDlg);
            break;
        }

        case WM_MOUSEMOVE:
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);
            int index = -1;

            for (int i = 0; i < 8; i++)
            {
                if (positions[i][0] < xPos && xPos < positions[i][2] && positions[i][1] < yPos && yPos < positions[i][3])
                {
                    index = i;
                }
            }

            if (hoverIndex != index)
            {
                hoverIndex = index;
                ShowFunctionInfo(hwndDlg, std::to_string(index + 1));
            }

            break;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDCANCEL:
                SetActionState("_REASONUS_SHOW_REASONUS_FUNCTION_WINDOW");
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
            SetActionState("_REASONUS_SHOW_REASONUS_FUNCTION_WINDOW");
            HideFunctionsDialog();
            break;
        }
        return 0;
    };

    void ShowFunctionsDialog()
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_8));
        file.read(ini);

        if (s_hwndReaSonusFunctionsDlg == NULL)
        {
            s_hwndReaSonusFunctionsDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_REASONUS_NATIVE_FUNCTIONS), g_hwnd, dlgProcFunctions);
        }
        ShowWindow(s_hwndReaSonusFunctionsDlg, SW_SHOW);
    }

    bool IsFunctionsDialogOpen()
    {
        return s_hwndReaSonusFunctionsDlg != NULL;
    }
}
