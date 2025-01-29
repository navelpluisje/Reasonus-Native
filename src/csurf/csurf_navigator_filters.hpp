#ifndef CSURF_NAVIGATOR_FILTERS_H_
#define CSURF_NAVIGATOR_FILTERS_H_

#include <map>
#include <string>

std::map<int, bool> GetSibblingTracks(std::map<int, bool> tracks);

std::map<int, bool> GetParentTracks(std::map<int, bool> tracks);

std::map<int, bool> GetChildTracks(std::map<int, bool> tracks);

bool FuzzyMatch(std::string trackName, std::string matches);

#endif