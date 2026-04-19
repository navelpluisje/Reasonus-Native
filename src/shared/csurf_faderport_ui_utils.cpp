#include "csurf_faderport_ui_utils.hpp"

#include <array>

#if __APPLE__ || __linux__
#include <WDL/swell/swell-types.h>
#include <WDL/swell/swell-functions.h>
#endif


void parseParms(const char *config, int parms[4]) {
    parms[0] = 0;
    parms[1] = 9;
    parms[2] = parms[3] = -1;

    const char *config_string = config;
    if (config_string != nullptr) {
        int params_length = 0;
        while (params_length < 4) {
            while (*config_string == ' ') {
                config_string++;
            }
            if ((*config_string < '0' || *config_string > '9') && *config_string != '-') {
                break;
            }

            parms[params_length++] = std::stoi(config_string);

            while (*config_string != 0 && *config_string != ' ') {
                config_string++;
            }
        }
    }
}

int AddComboEntry(HWND hwndDlg, const int lparam, char *buf, const int comboId) // NOLINT(*-const-correctness)
{
    int const wparam = static_cast<int>(SendDlgItemMessage(hwndDlg, comboId, CB_ADDSTRING, 0,
                                                           reinterpret_cast<LPARAM>(buf)));
    SendDlgItemMessage(hwndDlg, comboId, CB_SETITEMDATA, wparam, lparam);
    return wparam;
}

void AddListEntry(HWND hwndDlg, const std::string &buf, const int listId) {
    SendDlgItemMessage(hwndDlg, listId, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(buf.c_str()));
}
