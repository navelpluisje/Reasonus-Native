#ifndef CSURF_PLUGIN_MAPPING_UTILS_H_
#define CSURF_PLUGIN_MAPPING_UTILS_H_

#include "csurf_database.hpp"

std::string EscapeValue(std::string value);
std::string DeserializeValue(std::string value);

void PopulatePluginsDatabase(Database *db);
void PopulatePluginParameters(Database *db);

#endif