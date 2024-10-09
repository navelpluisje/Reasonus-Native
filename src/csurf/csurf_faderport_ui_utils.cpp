#include <WDL/swell/swell-types.h>
#include <swell/swell-functions.h>
#include "csurf_faderport_ui_utils.hpp"

void parseParms(const char *str, int parms[4])
{
    parms[0] = 0;
    parms[1] = 9;
    parms[2] = parms[3] = -1;

    const char *p = str;
    if (p)
    {
        int x = 0;
        while (x < 4)
        {
            while (*p == ' ')
                p++;
            if ((*p < '0' || *p > '9') && *p != '-')
                break;
            parms[x++] = atoi(p);
            while (*p && *p != ' ')
                p++;
        }
    }
}

int AddComboEntry(HWND hwndDlg, int x, char *buf, int comboId)
{
    int a = (int)SendDlgItemMessage(hwndDlg, comboId, CB_ADDSTRING, 0, (LPARAM)buf);
    SendDlgItemMessage(hwndDlg, comboId, CB_SETITEMDATA, a, x);
    return a;
}

void AddListEntry(HWND hwndDlg, string buf, int listId)
{
    SendDlgItemMessage(hwndDlg, listId, LB_ADDSTRING, 0, (LPARAM)buf.c_str());
}
