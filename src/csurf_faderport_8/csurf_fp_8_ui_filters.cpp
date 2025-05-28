#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_faderport_ui_utils.hpp"
#include "csurf_fp_8_ui_filters.hpp"
#include <mini/ini.h>

extern HWND g_hwnd;
extern REAPER_PLUGIN_HINSTANCE g_hInst;

namespace CSURF_FP_8_UI_FILTERS
{
    namespace
    {
        mINI::INIStructure ini;
        HWND s_hwndReaSonusFiltersDlg = NULL;
        std::vector<mINI::INIMap<std::string>> filtersDlgFilters;
        std::vector<std::string> filtersDlgFilterKeys;
        std::vector<std::string> filtersDlgFilterText;
        int filtersDlgSelectedFilter = -1;
        int filtersDlgSelectedFilterText = -1;
    }

    std::vector<std::string> GetFiltersKeys()
    {
        std::vector<std::string> filterKeys;

        int nbFilters = stoi(ini["filters"]["nb-filters"]);

        for (int i = 0; i < nbFilters; i++)
        {
            filterKeys.push_back(ini["filters"][std::to_string(i)]);
        }
        return filterKeys;
    }

    std::vector<std::string> GetFilterText()
    {
        std::string filterText = ini[filtersDlgFilterKeys[filtersDlgSelectedFilter]]["text"];
        std::vector<std::string> filterTextList = split(filterText, ",");
        return filterTextList;
    }

    void HideFiltersDialog()
    {
        ShowWindow(s_hwndReaSonusFiltersDlg, SW_HIDE);
        s_hwndReaSonusFiltersDlg = NULL;
    }

    void PopulateFiltersList(HWND hwndDlg, int select)
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

    void PopulateFilterTextList(HWND hwndDlg, int select)
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

    void PopulateFilter(HWND hwndDlg)
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

    void UpdateIniFiltersList()
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_8));

        ini["filters"].clear();
        ini["filters"]["nb-filters"] = std::to_string(filtersDlgFilterKeys.size());
        for (int i = 0; i < (int)filtersDlgFilterKeys.size(); i++)
        {
            ini["filters"][std::to_string(i)] = filtersDlgFilterKeys.at(i);
        }
        file.write(ini, true);
    }

    bool MoveFiltersListItem(FilterListDirection dir)
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

    void SaveCheckBoxValue(HWND hwndDlg, std::string key, int checkBox)
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_8));
        std::string filter = filtersDlgFilterKeys[filtersDlgSelectedFilter];

        ini[filter][key] = std::to_string(IsDlgButtonChecked(hwndDlg, checkBox));
        file.write(ini, true);
    }

    WDL_DLGRET dlgProcFiters(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
                mINI::INIFile file(GetReaSonusIniPath(FP_8));
                char buffer[255];

                GetDlgItemText(hwndDlg, IDC_EDIT_FilterText, buffer, std::size(buffer));
                filtersDlgFilterText.push_back(buffer);
                SetDlgItemText(hwndDlg, IDC_EDIT_FilterText, std::string("").c_str());
                std::string filter = filtersDlgFilterKeys[filtersDlgSelectedFilter];
                ini[filter]["text"] = join(filtersDlgFilterText, ",");

                file.write(ini, true);
                PopulateFilterTextList(hwndDlg, filtersDlgSelectedFilterText);
                break;
            }

            case IDC_BUTTON_REMOVE_TEXT:
            {
                mINI::INIFile file(GetReaSonusIniPath(FP_8));

                filtersDlgFilterText.erase(filtersDlgFilterText.begin() + filtersDlgSelectedFilterText);
                std::string filter = filtersDlgFilterKeys[filtersDlgSelectedFilter];
                ini[filter]["text"] = join(filtersDlgFilterText, ",");

                file.write(ini, true);
                PopulateFilterTextList(hwndDlg, filtersDlgSelectedFilterText - 1);
                break;
            }

            case IDC_BUTTON_ADD_FILTER:
            {
                mINI::INIFile file(GetReaSonusIniPath(FP_8));

                std::string newKey = GenerateUniqueKey("filter_");
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
                if (filtersDlgFilterKeys.size() == 0)
                {
                    break;
                }

                mINI::INIFile file(GetReaSonusIniPath(FP_8));
                std::string filter = filtersDlgFilterKeys[filtersDlgSelectedFilter];

                if (filter.empty())
                {
                    break;
                }

                char buffer[255];
                GetDlgItemText(hwndDlg, IDC_EDIT_FILTER_NAME, buffer, std::size(buffer));
                ini[filter]["name"] = buffer;
                file.write(ini, true);
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

    void ShowFiltersDialog()
    {
        mINI::INIFile file(GetReaSonusIniPath(FP_8));
        file.read(ini);

        filtersDlgFilterKeys = GetFiltersKeys();

        if (s_hwndReaSonusFiltersDlg == NULL)
        {
            s_hwndReaSonusFiltersDlg = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_REASONUS_NATIVE_FILTERS), g_hwnd, dlgProcFiters);
        }
        ShowWindow(s_hwndReaSonusFiltersDlg, SW_SHOW);
    }

    bool IsFiltersDialogOpen()
    {
        return s_hwndReaSonusFiltersDlg != NULL;
    }
}
