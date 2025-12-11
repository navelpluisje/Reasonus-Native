#ifndef CSURF_FADERPORT_UI_UTILS_H_
#define CSURF_FADERPORT_UI_UTILS_H_

#include <cstdio>
#include <string>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <WDL/win32_utf8.h>

#if __APPLE__ || __linux__
#include <WDL/swell/swell-types.h>
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define SystemOpenURL(url) std::system("start " url);
#elif __APPLE__
#define SystemOpenURL(url) std::system("open " url);
#elif __linux__
#define SystemOpenURL(url) std::system("xdg-open " url);
#else
#error "Unknown compiler"
#endif

extern HWND g_hwnd;

void parseParms(const char *str, int parms[4]);

int AddComboEntry(HWND hwndDlg, int x, char *buf, int comboId);

void AddListEntry(HWND hwndDlg, std::string buf, int listId);

#endif