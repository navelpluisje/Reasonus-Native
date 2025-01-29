#ifndef CSURF_FADERPORT_UI_FILTERS_H_
#define CSURF_FADERPORT_UI_FILTERS_H_

#include <string>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <WDL/win32_utf8.h>
#include <WDL/localize/localize.h>
#include <mini/ini.h>
#include <reaper_plugin_functions.h>
#include "csurf_utils.hpp"
#include "../../src/resource.h"
#include "csurf_faderport_ui_utils.hpp"

extern HWND g_hwnd;

enum FilterListDirection
{
    FilterListUp = -1,
    FilterListDown = 1,
};

namespace CSURF_FADERPORT_UI_FILTERS
{
    static mINI::INIStructure ini;
    static HWND s_hwndReaSonusFiltersDlg = NULL;
    static std::vector<mINI::INIMap<std::string>> filtersDlgFilters;
    static std::vector<std::string> filtersDlgFilterKeys;
    static std::vector<std::string> filtersDlgFilterText;
    static int filtersDlgSelectedFilter = -1;
    static int filtersDlgSelectedFilterText = -1;

    static std::vector<std::string> GetFiltersKeys()
    {
        std::vector<std::string> filterKeys;
        mINI::INIFile file(GetReaSonusIniPath());
        file.read(ini);

        int nbFilters = stoi(ini["filters"]["nb-filters"]);

        for (int i = 0; i < nbFilters; i++)
        {
            filterKeys.push_back(ini["filters"][std::to_string(i)]);
        }
        return filterKeys;
    }

    static std::vector<std::string> GetFilterText()
    {
        mINI::INIFile file(GetReaSonusIniPath());
        file.read(ini);

        std::string filterText = ini[filtersDlgFilterKeys[filtersDlgSelectedFilter]]["text"];
        std::vector<std::string> filterTextList = split(filterText, ",");
        return filterTextList;
    }

    static void HideFiltersDialog()
    {
        ShowWindow(s_hwndReaSonusFiltersDlg, SW_HIDE);
        s_hwndReaSonusFiltersDlg = NULL;
    }

    static void PopulateFiltersList(HWND hwndDlg, int select)
    {
        filtersDlgSelectedFilter = select;
        filtersDlgFilterKeys = GetFiltersKeys();

        SendMessage(GetDlgItem(hwndDlg, IDC_LIST_FILTERS), LB_RESETCONTENT, 0, 0);
        std::vector<std::string> keys = GetFiltersKeys();
        for (const std::string &key : keys)
        {
            AddListEntry(hwndDlg, ini[key]["name"], IDC_LIST_FILTERS);
        }
        SendMessage(GetDlgItem(hwndDlg, IDC_LIST_FILTERS), LB_SETCURSEL, select, 0);
    }

    static void PopulateFilterTextList(HWND hwndDlg, int select)
    {
        filtersDlgSelectedFilterText = select;
        filtersDlgFilterText = GetFilterText();

        SendMessage(GetDlgItem(hwndDlg, IDC_LIST_FILTER_TEXT), LB_RESETCONTENT, 0, 0);

        for (const std::string &value : filtersDlgFilterText)
        {
            AddListEntry(hwndDlg, value, IDC_LIST_FILTER_TEXT);
        }
        SendMessage(GetDlgItem(hwndDlg, IDC_LIST_FILTER_TEXT), LB_SETCURSEL, select, 0);
    }

    static void PopulateFilter(HWND hwndDlg)
    {
        if (filtersDlgSelectedFilter < 0)
        {
            SetDlgItemText(hwndDlg, IDC_EDIT_FILTER_NAME, std::string("").c_str());
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOW_SIBBLINGS), BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOW_PARENTS), BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOW_CHILDREN), BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOW_TOP_LEVEL), BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_MATCH_MULTIPLE), BM_SETCHECK, BST_UNCHECKED, 0);
            return;
        }
        std::string filterKey = filtersDlgFilterKeys[filtersDlgSelectedFilter];
        SetDlgItemText(hwndDlg, IDC_EDIT_FILTER_NAME, const_cast<char *>(ini[filterKey]["name"].c_str()));
        SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOW_SIBBLINGS), BM_SETCHECK, ini[filterKey]["sibblings"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOW_PARENTS), BM_SETCHECK, ini[filterKey]["parents"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOW_CHILDREN), BM_SETCHECK, ini[filterKey]["children"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_SHOW_TOP_LEVEL), BM_SETCHECK, ini[filterKey]["top-level"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
        SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_MATCH_MULTIPLE), BM_SETCHECK, ini[filterKey]["match-multiple"] == "1" ? BST_CHECKED : BST_UNCHECKED, 0);
        PopulateFilterTextList(hwndDlg, filtersDlgSelectedFilterText);
    }

    static void UpdateIniFiltersList()
    {
        mINI::INIFile file(GetReaSonusIniPath());

        ini["filters"].clear();
        ini["filters"]["nb-filters"] = std::to_string(filtersDlgFilterKeys.size());
        for (int i = 0; i < (int)filtersDlgFilterKeys.size(); i++)
        {
            ini["filters"][std::to_string(i)] = filtersDlgFilterKeys.at(i);
        }
        file.write(ini, true);
    }

    static bool MoveFiltersListItem(FilterListDirection dir)
    {
        if (filtersDlgSelectedFilter + dir < 0 || filtersDlgSelectedFilter + dir >= (int)filtersDlgFilterKeys.size())
        {
            return false;
        }

        std::string selectedFilter = filtersDlgFilterKeys[filtersDlgSelectedFilter];
        filtersDlgFilterKeys.erase(filtersDlgFilterKeys.begin() + filtersDlgSelectedFilter);
        filtersDlgFilterKeys.insert(filtersDlgFilterKeys.begin() + filtersDlgSelectedFilter + dir, selectedFilter);

        UpdateIniFiltersList();
        return true;
    }

    static void SaveCheckBoxValue(HWND hwndDlg, std::string key, int checkBox)
    {
        mINI::INIFile file(GetReaSonusIniPath());
        std::string filter = filtersDlgFilterKeys[filtersDlgSelectedFilter];

        ini[filter][key] = std::to_string(IsDlgButtonChecked(hwndDlg, checkBox));
        file.write(ini);
    }

    static WDL_DLGRET dlgProcFiters(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        (void)lParam;
        switch (uMsg)
        {
        case WM_INITDIALOG:
        {

            WDL_UTF8_HookListView(GetDlgItem(hwndDlg, IDC_LIST_FILTERS));
            WDL_UTF8_HookListView(GetDlgItem(hwndDlg, IDC_LIST_FILTER_TEXT));

            if (filtersDlgFilterKeys.size() > 0)
            {
                PopulateFiltersList(hwndDlg, 0);
                PopulateFilter(hwndDlg);
            }
            break;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {

            case IDC_LIST_FILTERS:
            {
                filtersDlgSelectedFilter = (int)SendDlgItemMessage(hwndDlg, IDC_LIST_FILTERS, LB_GETCURSEL, 0, 0);
                PopulateFilter(hwndDlg);
                break;
            }

            case IDC_LIST_FILTER_TEXT:
            {
                filtersDlgSelectedFilterText = (int)SendDlgItemMessage(hwndDlg, IDC_LIST_FILTER_TEXT, LB_GETCURSEL, 0, 0);
                break;
            }

            case IDC_BUTTON_ADD_TEXT:
            {
                mINI::INIFile file(GetReaSonusIniPath());
                char buffer[255];

                GetDlgItemText(hwndDlg, IDC_EDIT_FilterText, buffer, std::size(buffer));
                filtersDlgFilterText.push_back(buffer);
                SetDlgItemText(hwndDlg, IDC_EDIT_FilterText, std::string("").c_str());
                std::string filter = filtersDlgFilterKeys[filtersDlgSelectedFilter];
                ini[filter]["text"] = join(filtersDlgFilterText, ",");

                file.write(ini);
                PopulateFilterTextList(hwndDlg, filtersDlgSelectedFilterText);
                break;
            }

            case IDC_BUTTON_REMOVE_TEXT:
            {
                mINI::INIFile file(GetReaSonusIniPath());

                filtersDlgFilterText.erase(filtersDlgFilterText.begin() + filtersDlgSelectedFilterText);
                std::string filter = filtersDlgFilterKeys[filtersDlgSelectedFilter];
                ini[filter]["text"] = join(filtersDlgFilterText, ",");

                file.write(ini);
                PopulateFilterTextList(hwndDlg, filtersDlgSelectedFilterText - 1);
                break;
            }

            case IDC_BUTTON_ADD_FILTER:
            {
                mINI::INIFile file(GetReaSonusIniPath());

                std::string newKey = GenerateUniqueKey("Filter_");
                ini["filters"][ini["filters"]["nb-filters"]] = newKey;
                ini["filters"]["nb-filters"] = std::to_string(stoi(ini["filters"]["nb-filters"]) + 1);
                ini[newKey];
                ini[newKey]["name"] = "New Filter";
                ini[newKey]["text"] = "";
                ini[newKey]["sibblings"] = "0";
                ini[newKey]["parents"] = "0";
                ini[newKey]["children"] = "0";
                ini[newKey]["top-level"] = "0";
                ini[newKey]["match-multiple"] = "0";
                file.write(ini, true);

                PopulateFiltersList(hwndDlg, stoi(ini["filters"]["nb-filters"]) - 1);
                break;
            }

            case IDC_BUTTON_REMOVE_FILTER:
            {
                ini.remove(filtersDlgFilterKeys.at(filtersDlgSelectedFilter));
                filtersDlgFilterKeys.erase(filtersDlgFilterKeys.begin() + filtersDlgSelectedFilter);

                UpdateIniFiltersList();
                PopulateFiltersList(hwndDlg, 0);
                break;
            }

            case IDC_BUTTON_FILTER_ITEM_UP:
            {
                if (MoveFiltersListItem(FilterListUp))
                {
                    PopulateFiltersList(hwndDlg, filtersDlgSelectedFilter - 1);
                };
                break;
            }

            case IDC_EDIT_FILTER_NAME:
            {
                mINI::INIFile file(GetReaSonusIniPath());
                std::string filter = filtersDlgFilterKeys[filtersDlgSelectedFilter];

                if (filter.empty())
                {
                    break;
                }

                char buffer[255];
                GetDlgItemText(hwndDlg, IDC_EDIT_FILTER_NAME, buffer, std::size(buffer));
                ini[filter]["name"] = buffer;
                file.write(ini);
                break;
            }

            case IDC_BUTTON_FILTER_ITEM_DOWN:
            {
                if (MoveFiltersListItem(FilterListDown))
                {
                    PopulateFiltersList(hwndDlg, filtersDlgSelectedFilter + 1);
                }
                break;
            }

            case IDC_CHECK_SHOW_SIBBLINGS:
            {
                SaveCheckBoxValue(hwndDlg, "sibblings", IDC_CHECK_SHOW_SIBBLINGS);
                break;
            }

            case IDC_CHECK_SHOW_PARENTS:
            {
                SaveCheckBoxValue(hwndDlg, "parents", IDC_CHECK_SHOW_PARENTS);
                break;
            }

            case IDC_CHECK_SHOW_CHILDREN:
            {
                SaveCheckBoxValue(hwndDlg, "children", IDC_CHECK_SHOW_CHILDREN);
                break;
            }

            case IDC_CHECK_SHOW_TOP_LEVEL:
            {
                SaveCheckBoxValue(hwndDlg, "top-level", IDC_CHECK_SHOW_TOP_LEVEL);
                break;
            }

            case IDC_CHECK_MATCH_MULTIPLE:
            {
                SaveCheckBoxValue(hwndDlg, "match-multiple", IDC_CHECK_MATCH_MULTIPLE);
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

    static bool IsFiltersDialogOpen()
    {
        return s_hwndReaSonusFiltersDlg != NULL;
    }
} // namespace

#endif
