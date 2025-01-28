#ifndef CSURF_FADERPORT_UI_UTILS_H_
#define CSURF_FADERPORT_UI_UTILS_H_

#include <string>
#include <swell/swell-types.h>

using namespace std;

#include <cstdio>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define SystemOpenURL(url) std::system("start " url);
#elif __APPLE__
#define SystemOpenURL(url) std::system("open " url);
#elif __linux__
#define SystemOpenURL(url) std::system("xdg-open" url);
#else
#error "Unknown compiler"
#endif

void parseParms(const char *str, int parms[4]);

int AddComboEntry(HWND hwndDlg, int x, char *buf, int comboId);

void AddListEntry(HWND hwndDlg, string buf, int listId);

#endif