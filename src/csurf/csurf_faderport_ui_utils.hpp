#ifndef CSURF_FADERPORT_UI_UTILS_H_
#define CSURF_FADERPORT_UI_UTILS_H_

#include <string>
#include <WDL/swell/swell-types.h>
#include <swell/swell-functions.h>

using namespace std;

void parseParms(const char *str, int parms[4]);

int AddComboEntry(HWND hwndDlg, int x, char *buf, int comboId);

void AddListEntry(HWND hwndDlg, string buf, int listId);

#endif