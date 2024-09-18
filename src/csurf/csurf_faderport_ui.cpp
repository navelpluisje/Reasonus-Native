#include <wdltypes.h>
#include <win32_utf8.h>
#include <WDL/swell/swell-types.h>
#include <WDL/swell/swell-functions.h>
#include <reaper_plugin_functions.h>
#include "../src/resource.h"
#include <localize.h>

static void parseParms(const char *str, int parms[4])
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

static int AddComboEntry(HWND hwndDlg, int x, char *buf, int comboId)
{
    int a = (int)SendDlgItemMessage(hwndDlg, comboId, CB_ADDSTRING, 0, (LPARAM)buf);
    SendDlgItemMessage(hwndDlg, comboId, CB_SETITEMDATA, a, x);
    return a;
}

static WDL_DLGRET dlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        int parms[4];
        parseParms((const char *)lParam, parms);

        // ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT1), SW_HIDE);
        // ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT1_LBL), SW_HIDE);
        // ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT2), SW_HIDE);
        // ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT2_LBL), SW_HIDE);
        // ShowWindow(GetDlgItem(hwndDlg, IDC_EDIT2_LBL2), SW_HIDE);
        char buf[520];
        char *noDeviceString = const_cast<char *>(__LOCALIZE("No device selected", "reasonus-faderport"));

        WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_MidiIn));
        WDL_UTF8_HookComboBox(GetDlgItem(hwndDlg, IDC_COMBO_MidiOut));

        AddComboEntry(hwndDlg, 0, noDeviceString, IDC_COMBO_MidiIn);
        AddComboEntry(hwndDlg, 0, noDeviceString, IDC_COMBO_MidiOut);

        for (int i = 1; i <= GetNumMIDIInputs(); ++i)
            if (GetMIDIInputName(i, buf, sizeof(buf)))
            {
                int dev = AddComboEntry(hwndDlg, i, buf, IDC_COMBO_MidiIn);
                if (i == parms[2])
                {
                    SendDlgItemMessage(hwndDlg, IDC_COMBO_MidiIn, CB_SETCURSEL, dev, 0);
                }
            }

        for (int i = 1; i <= GetNumMIDIOutputs(); ++i)

            if (GetMIDIOutputName(i, buf, sizeof(buf)))
            {
                int dev = AddComboEntry(hwndDlg, i, buf, IDC_COMBO_MidiOut);
                if (i == parms[3])
                {
                    SendDlgItemMessage(hwndDlg, IDC_COMBO_MidiOut, CB_SETCURSEL, dev, 0);
                }
            }
    }
    break;
    case WM_USER + 1024:
        if (wParam > 1 && lParam)
        {
            char tmp[512];

            int indev = -1, outdev = -1;
            int r = SendDlgItemMessage(hwndDlg, IDC_COMBO_MidiIn, CB_GETCURSEL, 0, 0);
            if (r != CB_ERR)
                indev = SendDlgItemMessage(hwndDlg, IDC_COMBO_MidiIn, CB_GETITEMDATA, r, 0);
            r = SendDlgItemMessage(hwndDlg, IDC_COMBO_MidiOut, CB_GETCURSEL, 0, 0);
            if (r != CB_ERR)
                outdev = SendDlgItemMessage(hwndDlg, IDC_COMBO_MidiOut, CB_GETITEMDATA, r, 0);

            snprintf(tmp, 100, "0 0 %d %d", indev, outdev);
            lstrcpyn((char *)lParam, tmp, wParam);
        }
        break;
    }
    return 0;
};
