#ifndef CSURF_FADERPORT_UI_FILTERS_H_
#define CSURF_FADERPORT_UI_FILTERS_H_

#include <wdltypes.h>
#include <win32_utf8.h>
#include <localize.h>
#include <string>
#include <WDL/swell/swell-types.h>
#include <reaper_plugin_functions.h>
#include "../src/resource.h"
#include "extern/ini.hpp"
#include "csurf_utils.hpp"
#include "csurf_faderport_ui_utils.hpp"
#include <localize/localize.h>
#include <swell/swell-functions.h>

extern HWND g_hwnd;

using namespace std;

namespace CSURF_FADERPORT_UI_FILTERS
{
    static mINI::INIStructure ini;
    static HWND s_hwndReaSonusFiltersDlg = NULL;
    static vector<mINI::INIMap<string>> filtersDlgFilters;
    static vector<string> filtersDlgFilterKeys;
    static int filtersDlgSelectedFilter = -1;

    static vector<string> GetFiltersKeys()
    {
        vector<string> filterKeys;
        mINI::INIFile file(GetReaSonusIniPath());
        file.read(ini);

        int nbFilters = stoi(ini["filters"]["nb-filters"]);

        for (int i = 0; i < nbFilters; i++)
        {
            filterKeys.push_back(ini["filters"][to_string(i)]);
        }
        return filterKeys;
    }

    static string GenerateFilterKey()
    {
        srand(time(0));
        string p = "filter_";
        char a[] = "abcdefghijklmnopqrstuvwxyz";
        for (int i = 0; i < 24; i++)
        {
            char rnd = a[(rand() + 100) % 26];
            p += rnd;
        }
        return p;
    }

    static void HideFiltersDialog()
    {
        ShowWindow(s_hwndReaSonusFiltersDlg, SW_HIDE);
        s_hwndReaSonusFiltersDlg = NULL;
    }

    static void PopulateFilter(HWND hwndDlg)
    {
        if (filtersDlgSelectedFilter < 0)
        {
            SetDlgItemText(hwndDlg, IDC_EDIT_FilterName, "");
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ShowSibblings), BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ShowParents), BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ShowChildren), BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ShowTopLevel), BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_MatchMultiple), BM_SETCHECK, BST_UNCHECKED, 0);
            return;
        }
        string filterKey = filtersDlgFilterKeys[filtersDlgSelectedFilter];
        SetDlgItemText(hwndDlg, IDC_EDIT_FilterName, const_cast<char *>(ini[filterKey]["name"].c_str()));
        SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ShowSibblings), BM_SETCHECK, ini[filterKey]["sibblings"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ShowParents), BM_SETCHECK, ini[filterKey]["parents"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ShowChildren), BM_SETCHECK, ini[filterKey]["children"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_ShowTopLevel), BM_SETCHECK, ini[filterKey]["top-level"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_MatchMultiple), BM_SETCHECK, ini[filterKey]["match-multiple"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
    }

    static void PopulateFiltersList(HWND hwndDlg)
    {
        SendMessage(GetDlgItem(hwndDlg, IDC_LIST_Filters), LB_RESETCONTENT, 0, 0);
        vector<string> keys = GetFiltersKeys();
        for (const string &key : keys)
        {
            AddListEntry(hwndDlg, key, IDC_LIST_Filters);
        }
    }

    static WDL_DLGRET dlgProcFiters(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        (void)lParam;
        switch (uMsg)
        {
        case WM_INITDIALOG:
        {

            WDL_UTF8_HookListView(GetDlgItem(hwndDlg, IDC_LIST_Filters));
            WDL_UTF8_HookListView(GetDlgItem(hwndDlg, IDC_LIST_FilterText));

            if (filtersDlgFilterKeys.size() > 0)
            {
                PopulateFiltersList(hwndDlg);
                SendMessage(GetDlgItem(hwndDlg, IDC_LIST_Filters), LB_SETCURSEL, 0, 0);
                filtersDlgSelectedFilter = 0;
                PopulateFilter(hwndDlg);
            }

            break;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {

            case IDC_LIST_Filters:
            {
                filtersDlgSelectedFilter = (int)SendDlgItemMessage(hwndDlg, IDC_LIST_Filters, LB_GETCURSEL, 0, 0);
                PopulateFilter(hwndDlg);
                break;
            }

            case IDC_BUTTON_ADD_TEXT:
            {
                char buffer[255];
                GetDlgItemText(hwndDlg, IDC_EDIT_FilterText, buffer, size(buffer));
                AddListEntry(hwndDlg, buffer, IDC_LIST_FilterText);
                SetDlgItemText(hwndDlg, IDC_EDIT_FilterText, const_cast<char *>(""));

                // Add a new Item to the text list
                break;
            }

            case IDC_BUTTON_REMOVE_TEXT:
            {
                char buffer[255];

                int textIndex = (int)SendDlgItemMessage(hwndDlg, IDC_LIST_FilterText, LB_GETSEL, 0, 0);
                logInteger("Index", textIndex);
                // RemoveListEntry(hwndDlg, textIndex, IDC_LIST_FilterText);
                GetDlgItemText(hwndDlg, IDC_LIST_FilterText, buffer, size(buffer));

                // SendMessage(GetDlgItem(hwndDlg, IDC_EDIT_Function1), LB_GETITEMDATA, textIndex, 0);

                ShowConsoleMsg(buffer);
                // Add a new Item to the text list
                break;
            }

            case IDC_BUTTON_ADD:
            {
                mINI::INIFile file(GetReaSonusIniPath());

                string newKey = GenerateFilterKey();
                ini["filters"][ini["filters"]["nb-filters"]] = newKey;
                ini["filters"]["nb-filters"] = to_string(stoi(ini["filters"]["nb-filters"]) + 1);
                ini[newKey];
                ini[newKey]["name"] = "New Filter";
                ini[newKey]["text"] = "";
                ini[newKey]["sibblings"] = "0";
                ini[newKey]["parents"] = "0";
                ini[newKey]["children"] = "0";
                ini[newKey]["top-level"] = "0";
                ini[newKey]["match-multiple"] = "0";
                file.write(ini, true);

                filtersDlgFilterKeys = GetFiltersKeys();
                PopulateFiltersList(hwndDlg);
                SendMessage(GetDlgItem(hwndDlg, IDC_LIST_Filters), LB_SETCURSEL, stoi(ini["filters"]["nb-filters"]) - 1, 0);
                break;
            }

            case IDC_BUTTON_REMOVE:
            {
                mINI::INIFile file(GetReaSonusIniPath());

                ini.remove(filtersDlgFilterKeys.at(filtersDlgSelectedFilter));
                filtersDlgFilterKeys.erase(filtersDlgFilterKeys.begin() + filtersDlgSelectedFilter);

                ini["filters"].clear();
                ini["filters"]["nb-filters"] = to_string(filtersDlgFilterKeys.size());
                for (int i = 0; i < (int)filtersDlgFilterKeys.size(); i++)
                {
                    ini["filters"][to_string(i)] = filtersDlgFilterKeys.at(i);
                }
                file.write(ini, true);

                filtersDlgFilterKeys = GetFiltersKeys();
                PopulateFiltersList(hwndDlg);
                SendMessage(GetDlgItem(hwndDlg, IDC_LIST_Filters), LB_SETCURSEL, 0, 0);

                break;
            }

            case IDC_BUTTON_UP:
            {
                // move current selected item 1 up
                break;
            }

            case IDC_BUTTON_DOWN:
            {
                // move current selected item 1 down
                break;
            }

            // Handle save click
            case IDOK:
            {
                // save the filters to the ini again
                break;
            }

            // Handle Cancel click
            case IDCANCEL:
                HideFiltersDialog();
                break;
            }
            break;

        case WM_CLOSE:
            HideFiltersDialog();
            break;
        }
        return 0;
    };

    static void ShowFiltersDialog()
    {
        filtersDlgFilterKeys = GetFiltersKeys();

        if (s_hwndReaSonusFiltersDlg == NULL)
        {
            s_hwndReaSonusFiltersDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_REASONUS_NATIVE_FILTERS), g_hwnd, dlgProcFiters);
        }
        ShowWindow(s_hwndReaSonusFiltersDlg, SW_SHOW);
    }
} // namespace

#endif
