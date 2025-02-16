#ifndef CSURF_FADERPORT_UI_PLUGIN_EDIT_H_
#define CSURF_FADERPORT_UI_PLUGIN_EDIT_H_

#include <string>
// #include <WDL/wdltypes.h> // might be unnecessary in future
// #include <WDL/win32_utf8.h>
// #include <reaper_plugin_functions.h>
#include <mini/ini.h>

extern HWND g_hwnd;

enum FilterListDirection
{
    FilterListUp = -1,
    FilterListDown = 1,
};

namespace CSURF_FADERPORT_UI_PLUGIN_EDIT
{
    void HidePluginEditDialog();

    void PopulateFields(HWND hwndDlg);

    void StoreField(std::string controlName, std::string fieldName, std::string value);

    WDL_DLGRET dlgProcFiters(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void ShowPluginEditDialog(std::string _fileName, int index);

    bool IsPluginEditDialogOpen();
} // namespace

#endif
