#ifndef CSURF_FADERPORT_UI_FILTERS_H_
#define CSURF_FADERPORT_UI_FILTERS_H_

#include "../resource.h"
#include <string>
#include <vector>

extern HWND g_hwnd;

enum FilterListDirection
{
    FilterListUp = -1,
    FilterListDown = 1,
};

namespace CSURF_FADERPORT_UI_FILTERS
{
    std::vector<std::string> GetFiltersKeys();

    std::vector<std::string> GetFilterText();

    void HideFiltersDialog();

    void PopulateFiltersList(HWND hwndDlg, int select);

    void PopulateFilterTextList(HWND hwndDlg, int select);

    void PopulateFilter(HWND hwndDlg);

    void UpdateIniFiltersList();

    bool MoveFiltersListItem(FilterListDirection dir);

    void SaveCheckBoxValue(HWND hwndDlg, std::string key, int checkBox);

    WDL_DLGRET dlgProcFiters(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void ShowFiltersDialog();

    bool IsFiltersDialogOpen();
}

#endif
