#ifndef CSURF_FP_UI_FUNCTIONS_V2_H_
#define CSURF_FP_UI_FUNCTIONS_V2_H_

#include "../resource.h"
#include <string>

extern HWND g_hwnd;

namespace CSURF_FP_V2_UI_FUNCTIONS
{
    void PopulateActionFields(HWND hwndDlg);

    void mytimer();

    void PromptForFunctionAction(HWND hwndDlg, std::string index);

    void ShowFunctionInfo(HWND hwndDlg, std::string index);

    void HideFunctionsDialog();

    WDL_DLGRET dlgProcFunctions(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void ShowFunctionsDialog();

    bool IsFunctionsDialogOpen();
}

#endif