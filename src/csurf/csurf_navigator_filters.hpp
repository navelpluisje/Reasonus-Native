#ifndef CSURF_NAVIGATOR_FILTERS_H_
#define CSURF_NAVIGATOR_FILTERS_H_

#include <map>
#include <string>

using namespace std;

map<int, bool> GetSibblingTracks(map<int, bool> tracks);

map<int, bool> GetParentTracks(map<int, bool> tracks);

map<int, bool> GetChildTracks(map<int, bool> tracks);

bool FuzzyMatch(string trackName, string matches);

#endif