#include "../resource.h"
#include "csurf_utils.hpp"
#include "csurf_faderport_ui_utils.hpp"

extern REAPER_PLUGIN_HINSTANCE g_hInst;

namespace CSURF_FADERPORT_UI_PLUGIN_EDIT
{
    namespace
    {
        mINI::INIStructure ini;
        HWND s_hwndReaSonusPluginEditDlg = NULL;
        std::string fileName;
        std::string controlIndex;
    }

    void HidePluginEditDialog()
    {
        ShowWindow(s_hwndReaSonusPluginEditDlg, SW_HIDE);
        s_hwndReaSonusPluginEditDlg = NULL;
    }

    void PopulateFields(HWND hwndDlg)
    {
        mINI::INIFile file(fileName);
        file.read(ini);

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

    void StoreField(std::string controlName, std::string fieldName, std::string value)
    {
        if (ini[controlName][fieldName] != value)
        {
            mINI::INIFile file(fileName);
            ini[controlName][fieldName] = value;
            file.write(ini);
        }
    }

    WDL_DLGRET dlgProcFiters(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

    void ShowPluginEditDialog(std::string _fileName, int index)
    {
        fileName = _fileName;
        controlIndex = std::to_string(index);

        if (s_hwndReaSonusPluginEditDlg == NULL)
        {
            s_hwndReaSonusPluginEditDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_REASONUS_NATIVE_PLUGIN_EDIT), g_hwnd, dlgProcFiters);
        }
        ShowWindow(s_hwndReaSonusPluginEditDlg, SW_SHOW);
    }

    bool IsPluginEditDialogOpen()
    {
        return s_hwndReaSonusPluginEditDlg != NULL;
    }
} // namespace
