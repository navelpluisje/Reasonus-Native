#include "csurf_plugin_utils.hpp"

#include <regex>

#include "csurf_daw.hpp"
#include "reaper_plugin_functions.h"
#include "csurf_utils.hpp"
#include "../ui/csurf_ui_colors.hpp"
#include "../i18n/i18n.hpp"

std::string PluginUtils::GetPluginFolderPath() {
    return createPathName({std::string(GetResourcePath()), "ReaSonus", "Plugins"});
}

std::string PluginUtils::GetPluginCacheFolderPath() {
    return createPathName({std::string(GetResourcePath()), "ReaSonus", "PluginsCache"});
}

std::string PluginUtils::GetReaSonusPluginPath(
    std::string developer,
    const std::string &plugin_name,
    const std::string &plugin_type,
    const bool create
) {
    const std::string path = createPathName({GetPluginFolderPath(), std::move(developer)});

    if (create) {
        createPathIfNotExist(path);
    }

    return createPathName({path, plugin_name + "." + plugin_type + ".ini"});
}

std::string PluginUtils::GetReaSonusPluginCachePath(
    std::string developer,
    const std::string &plugin_name,
    const std::string &plugin_type,
    const bool create
) {
    const std::string path = createPathName({GetPluginCacheFolderPath(), std::move(developer)});

    if (create) {
        createPathIfNotExist(path);
    }

    return createPathName({path, plugin_name + "." + plugin_type + ".ini"});
}

std::string PluginUtils::StripPluginName(const std::string &plugin_name) {
    std::vector<std::string> pluginNameParts = split(
        StripPluginNamePrefixes(StripPluginChannelPostfix(plugin_name.data()).data()), " ("
    );

    if (pluginNameParts.size() > 1) {
        pluginNameParts.pop_back();
    }

    return join(pluginNameParts, " (");
}

std::string PluginUtils::StripPluginDeveloper(const std::string &plugin_name) {
    const std::vector<std::string> pluginNameParts = split(
        StripPluginNamePrefixes(StripPluginChannelPostfix(plugin_name.data()).data()), " ("
    );

    std::string developer = pluginNameParts.at(pluginNameParts.size() - 1);
    if (!developer.empty()) {
        developer.pop_back();
    }

    return developer;
}

std::string PluginUtils::StripPluginNamePrefixes(char const *name) {
    std::vector<std::string> splitted_name = split(std::string(name), PREFIX_SEPARATOR);

    if (splitted_name.empty()) {
        return {name};
    }

    return splitted_name[splitted_name.size() - 1];
}

std::string PluginUtils::StripPluginChannelPostfix(char const *name) {
    std::vector<std::string> splitted_name = split(std::string(name), " (");
    splitted_name.pop_back();

    return join(splitted_name, " (");
}

std::string PluginUtils::ExtractDeveloperName(const std::string &plugin_name) {
    const std::vector<std::string> plugin_name_parts = split(plugin_name, " (");
    std::string developer;
    const int max_index = plugin_name_parts.size() - 1;
    const std::regex regex(".*(^[0-9->]{1,}ch|^[0-9]{1,3}[\\s]out|^mono)\\)");

    for (int i = max_index; i > 0; i--) {
        if (!std::regex_match(plugin_name_parts.at(i), regex)) {
            developer = plugin_name_parts.at(i);
            break;
        }
    }

    if (!developer.empty()) {
        developer.pop_back();
    }

    return developer;
}

std::string PluginUtils::ExtractPluginName(const std::string &plugin_name) {
    const std::string developer = ExtractDeveloperName(plugin_name);

    std::vector<std::string> plugin_name_parts = split(
        StripPluginNamePrefixes(StripPluginChannelPostfix(plugin_name.c_str()).data()),
        " (" + developer
    );

    if (plugin_name_parts.size() > 1) {
        plugin_name_parts.pop_back();
    }

    return join(plugin_name_parts, " (" + developer);
}

std::string PluginUtils::ExtractPluginType(const std::string &plugin_name) {
    const std::vector<std::string> plugin_name_parts = split(plugin_name, ": ");

    return toLowerCase(plugin_name_parts.at(0));
}

std::string PluginUtils::GetPluginsPath() {
    return createPathName({std::string(GetResourcePath()), "ReaSonus", "Plugins"});
}

bool PluginUtils::hasPluginConfigFile(MediaTrack *media_track, const int pluginId) {
    const std::string plugin_name = DAW::GetTrackFxName(media_track, pluginId, false);
    const std::string developer_name = DAW::GetTrackFxDeveloper(media_track, pluginId);
    const std::string plugin_type = DAW::GetTrackFxType(media_track, pluginId);

    return file_exists(GetReaSonusPluginPath(developer_name, plugin_name, plugin_type, false).c_str());
}

bool PluginUtils::IsPluginFX(std::string plugin_name) {
    const int pos = static_cast<int>(plugin_name.find(PREFIX_SEPARATOR));
    // If we can not find the delimiter, we can not determine the type of plugin, so asume it's an effect
    if (pos < 0) {
        return true;
    }

    plugin_name.erase(pos, plugin_name.length());
    plugin_name.erase(0, plugin_name.length() - 1);

    return plugin_name != "i";
}

std::vector<std::string> PluginUtils::GetpluginDevelopers(const bool sorted) {
    std::vector<std::string> developers;

    bool has_next = true;
    int index = 0;

    while (has_next) {
        const char *name = EnumerateSubdirectories(GetPluginsPath().c_str(), index);
        if (name == nullptr) {
            has_next = false;
        } else {
            index++;
            const std::vector<std::string> splitted_name = split(name, ".");
            developers.emplace_back(name);
        }
    }
    if (sorted) {
        std::sort(developers.begin(), developers.end());
    }

    return developers;
}

std::vector<std::string> PluginUtils::GetDeveloperPlugins(std::string developer, const bool sorted) {
    const std::string path = createPathName({GetPluginsPath(), std::move(developer)});
    std::vector<std::string> developer_plugins;

    bool has_next = true;
    int index = 0;

    while (has_next) {
        const char *name = EnumerateFiles(path.c_str(), index);

        if (name == nullptr) {
            has_next = false;
        } else {
            index++;
            std::vector<std::string> splitted_name = split(std::string(name), ".");

            if (splitted_name[splitted_name.size() - 1] == "ini") {
                developer_plugins.emplace_back(name);
            }
        }
    }

    if (sorted) {
        std::sort(developer_plugins.begin(), developer_plugins.end());
    }

    return developer_plugins;
}

std::vector<std::string> PluginUtils::GetInstalledPlugins() {
    bool has_next = true;
    int index = 0;
    const char *plugin_name;
    const char *plugin_indent;
    std::vector<std::string> installed_plugins;

    while (has_next) {
        if (EnumInstalledFX(index, &plugin_name, &plugin_indent)) {
            installed_plugins.emplace_back(plugin_name);
            index++;
        } else {
            has_next = false;
        }
    }

    return installed_plugins;
}

std::string PluginUtils::createCategoryName(const std::string &plugin_name, std::string plugin_type) {
    plugin_type = toLowerCase(plugin_type);
    const std::vector<std::string> name_parts = split(
        StripPluginName(plugin_name), " "
    );
    return join(name_parts, "_") + "." + replace(plugin_type, "i", "");
}

std::vector<PluginMeta> PluginUtils::ExtractInstalledPluginMeta(
    std::set<std::string> &developers,
    std::set<std::string> &plugin_types,
    std::set<std::string> &plugin_categories
) {
    const std::vector<std::string> installed_plugins = GetInstalledPlugins();
    std::vector<PluginMeta> plugin_meta;
    mINI::INIStructure category_file;
    const mINI::INIFile file(createPathName({GetResourcePath(), "reaper-fxtags.ini"}));
    file.read(category_file);

    for (const auto &plugin_name: installed_plugins) {
        PluginMeta meta(plugin_name);
        std::string developer = ExtractDeveloperName(plugin_name);
        if (developer.empty()) {
            continue;
        }
        const std::string plugin_type = ExtractPluginType(plugin_name);
        const std::string short_name = StripPluginName(plugin_name);
        const std::string cat_name = createCategoryName(plugin_name, plugin_type);

        if (plugin_type == "js") {
            developer = "js";
        }

        developers.emplace(developer);
        plugin_types.emplace(plugin_type);
        plugin_categories.emplace(cat_name);
        meta.SetDeveloper(developer);
        meta.SetFullName(plugin_name);
        meta.SetShortName(short_name);
        meta.SetCategory(category_file.get("category").get(cat_name));
        meta.SetPluginType(toLowerCase(plugin_type));

        plugin_meta.emplace_back(meta);
    }

    return plugin_meta;
}

std::vector<PluginMeta> PluginUtils::ExtractInstalledPluginMeta(
    std::set<std::string> &developers
) {
    std::set<std::string> plugin_types;
    std::set<std::string> plugin_categories;
    return ExtractInstalledPluginMeta(developers, plugin_types, plugin_categories);
}

std::vector<std::string> PluginUtils::GetPluginTypes() {
    std::vector<std::string> plugin_types = {
        "vst",
        "vsti",
        "vst3",
        "vst3i",
        "au",
        "aui",
        "clap",
        "clapi",
        "lv2",
        "lv2i"
    };

    return plugin_types;
}

std::map<std::string, std::string> PluginUtils::GetPluginTypesMap() {
    std::map<std::string, std::string> plugin_types_map = {
        {"vst", "V"},
        {"vsti", "Vi"},
        {"vst3", "3"},
        {"vst3i", "3i"},
        {"au", "A"},
        {"aui", "Ai"},
        {"clap", "C"},
        {"clapi", "Ci"},
        {"lv2", "L"},
        {"lv2i", "Li"}
    };

    return plugin_types_map;
}

std::string PluginUtils::GetShortPluginType(const std::string &plugin_type) {
    std::map<std::string, std::string> type_map = GetPluginTypesMap();

    if (type_map.find(plugin_type) == type_map.end()) {
        return "X";
    }

    return type_map[plugin_type];
}

int PluginUtils::GetPluginTypeBgColors(const std::string &plugin_type) {
    std::map<std::string, int> bg_colors = {
        {"vst", UI_COLORS::Accent},
        {"vsti", UI_COLORS::Accent},
        {"vst3", UI_COLORS::Tomato},
        {"vst3i", UI_COLORS::Tomato},
        {"au", UI_COLORS::Blue},
        {"aui", UI_COLORS::Blue},
        {"clap", UI_COLORS::Grey},
        {"clapi", UI_COLORS::Grey},
        {"lv2", UI_COLORS::Olive},
        {"lv2i", UI_COLORS::Olive}
    };

    if (bg_colors.find(plugin_type) == bg_colors.end()) {
        return UI_COLORS::White;
    }

    return bg_colors[plugin_type];
}

int PluginUtils::GetPluginTypeFgColors(const std::string &plugin_type) {
    std::map<std::string, int> fg_colors = {
        {"vst", UI_COLORS::Main_18},
        {"vsti", UI_COLORS::Main_18},
        {"vst3", UI_COLORS::White},
        {"vst3i", UI_COLORS::White},
        {"au", UI_COLORS::Main_18},
        {"aui", UI_COLORS::Main_18},
        {"clap", UI_COLORS::White},
        {"clapi", UI_COLORS::White},
        {"lv2", UI_COLORS::Main_18},
        {"lv2i", UI_COLORS::Main_18}
    };

    if (fg_colors.find(plugin_type) == fg_colors.end()) {
        return UI_COLORS::Main_18;
    }

    return fg_colors[plugin_type];
}

std::string PluginUtils::FormatPluginType(const std::string &value) {
    std::string plugin_type = toUpperCase(value);

    if (value.back() == 'i') {
        plugin_type.pop_back();
        plugin_type.push_back('i');
    }

    return plugin_type;
}

std::string PluginUtils::GetPluginRequestString(const std::string &plugin_origname, std::string plugin_type) {
    std::vector<std::string> allowed_plugin_types = GetPluginTypes();
    const auto it_type = std::find(allowed_plugin_types.begin(), allowed_plugin_types.end(), plugin_type);

    // Not found, the plugin type has either not been set or is invalid!
    if (it_type == allowed_plugin_types.end()) {
        return "";
    }

    // ------------------------------------------------------------------------
    // VST actually means version 1 of the VST spec. Most plugins nowadays are
    // using version 2.x of the VST spec. VST2 is what needs to be used during
    // plugin insertion, otherwise no plugin will be found unless there is an
    // alternate version also installed.
    // ------------------------------------------------------------------------
    if (plugin_type == "vst" || plugin_type == "vsti") {
        plugin_type.replace(0, 3, "VST2");
    }

    // Make sure the properly formatted type is included when adding a plugin!
    std::string plugin_name_with_type = FormatPluginType(plugin_type) + ": " + plugin_origname;

    return plugin_name_with_type;
}

std::string PluginUtils::GetPluginRequestString(
    const std::string &developer,
    const std::string &plugin_name,
    const std::string &plugin_type
) {
    mINI::INIStructure plugin_params;
    const mINI::INIFile file(GetReaSonusPluginPath(developer, plugin_name, plugin_type, false));
    file.read(plugin_params);

    return GetPluginRequestString(
        plugin_params["global"]["origname"],
        plugin_params["global"]["type"]
    );
}

bool PluginUtils::DeletePluginMappingFile(
    const std::string &developer,
    const std::string &plugin_name,
    const std::string &plugin_type
) {
    const std::string mapping_path = GetReaSonusPluginPath(developer, plugin_name, plugin_type, true);
    const std::filesystem::path mapping_folder = createPathName({GetPluginFolderPath(), developer});

    std::string message = I18n::GetInstance()->t(
        "mapping",
        "confirm.remove-mapping.message",
        plugin_name
    );

    if (MB(
            replaceAll(message, "\\n", "\n").c_str(),
            I18n::GetInstance()->t("mapping", "confirm.remove-mapping.title").c_str(),
            1) == 1
    ) {
        std::remove(mapping_path.c_str());

        if (std::filesystem::is_empty(mapping_folder)) {
            std::filesystem::remove(mapping_folder);
        }

        if (HasPluginMappingCache(developer, plugin_name, plugin_type)) {
            DeletePluginMappingCacheFile(developer, plugin_name, plugin_type);
        }

        return true;
    }

    return false;
}

bool PluginUtils::CreatePluginMappingCacheFile(MediaTrack *media_track, int plugin_id, bool update) {
    char plugin_name[256] = ""; // NOLINT(*-avoid-c-arrays)
    mINI::INIStructure cache;

    if (!TrackFX_GetFXName(media_track, plugin_id, plugin_name, std::size(plugin_name))) {
        return false;
    }

    const int num_params = TrackFX_GetNumParams(media_track, plugin_id);
    const std::string developer = ExtractDeveloperName(plugin_name);
    const std::string short_name = ExtractPluginName(plugin_name);
    const std::string plugin_type = ExtractPluginType(plugin_name);
    const std::string cache_path = GetReaSonusPluginCachePath(developer, short_name, plugin_type, true);

    cache["global"];
    cache["global"]["origname"] = StripPluginNamePrefixes(plugin_name);
    cache["global"]["name"] = short_name;
    cache["global"]["type"] = plugin_type;
    cache["global"]["developer"] = developer;
    cache["id"];
    cache["params"];
    cache["steps"];

    std::vector<int> param_id;
    std::vector<std::string> param_name;
    std::vector<int> param_steps;

    for (int i = 0; i < num_params; i++) {
        param_id.push_back(i);
        param_name.push_back(DAW::GetTrackFxParamName(media_track, plugin_id, i));
        param_steps.push_back(DAW::GetTrackFxParamNbSteps(media_track, plugin_id, i));
    }

    const std::vector<std::string> param_filters = GetParamFilters(developer, short_name, plugin_type);
    WriteFilterParams(param_filters, param_id, param_name, param_steps, cache);

    const mINI::INIFile file(cache_path);

    if (update) {
        return file.write(cache, true);
    }

    return file.generate(cache, true);
}

bool PluginUtils::UpdatePluginMappingCacheFile(const std::string &full_plugin_name) {
    InsertTrackAtIndex(0, false);
    MediaTrack *media_track = GetTrack(nullptr, 0);
    TrackFX_AddByName(media_track, full_plugin_name.c_str(), false, -1);

    const bool success = CreatePluginMappingCacheFile(media_track, 0, true);
    DeleteTrack(media_track);

    return success;
}

bool PluginUtils::UpdatePluginMappingCacheFileByDeveloper(const std::string &developer) {
    const std::vector<std::string> plugins = GetDeveloperPlugins(developer, false);
    mINI::INIStructure plugin_mapping_ini;
    std::string file_name;

    for (int i = 0; i < static_cast<int>(plugins.size()); i++) {
        mINI::INIFile file(createPathName({GetPluginFolderPath(), developer, plugins[i]}));
        if (!file.read(plugin_mapping_ini)) {
            continue;
        }

        UpdatePluginMappingCacheFile(GetPluginRequestString(
            plugin_mapping_ini["global"]["origname"],
            plugin_mapping_ini["global"]["type"]
        ));
    }

    return true;
}

void PluginUtils::DeletePluginMappingCacheFile(
    const std::string &developer,
    const std::string &plugin_name,
    const std::string &plugin_type
) {
    const std::string cache_path = GetReaSonusPluginCachePath(developer, plugin_name, plugin_type, true);
    const std::filesystem::path cache_folder = createPathName({GetPluginCacheFolderPath(), developer});

    std::remove(cache_path.c_str());

    if (std::filesystem::is_empty(cache_folder)) {
        std::filesystem::remove(cache_folder);
    }
}

mINI::INIStructure PluginUtils::GetPluginMappingCacheStructure(
    const std::string &developer,
    const std::string &plugin_name,
    const std::string &plugin_type
) {
    mINI::INIStructure plugin_cache;
    const mINI::INIFile file(GetReaSonusPluginCachePath(developer, plugin_name, plugin_type, false));
    file.read(plugin_cache);

    return plugin_cache;
}

bool PluginUtils::HasPluginMappingCache(
    const std::string &developer,
    const std::string &plugin_name,
    const std::string &plugin_type
) {
    return file_exists(GetReaSonusPluginCachePath(developer, plugin_name, plugin_type, false).c_str());
}

bool PluginUtils::HasPluginMappingCache(const std::string &cache_path) {
    return file_exists(cache_path.c_str());
}

bool PluginUtils::AddDeveloperParamFilter(std::string developer_name, std::string fiter_name) {
    mINI::INIStructure developer_filter_ini;
    const std::string developer_filter_path = createPathName({
        GetPluginCacheFolderPath(),
        "FilterDeveloper.ini"
    });

    const mINI::INIFile developer_filter_file(developer_filter_path);

    if (developer_filter_file.read(developer_filter_ini)) {
        if (!developer_filter_ini.has(developer_name)) {
            developer_filter_ini[developer_name];
        }
        const int size = developer_filter_ini[developer_name].size();
        developer_filter_ini[developer_name][std::to_string(size)] = fiter_name;

        developer_filter_file.write(developer_filter_ini, true);
        return true;
    }
    return false;
}

std::vector<std::string> PluginUtils::GetParamFilters(
    const std::string &developer_name,
    const std::string &plugin_name,
    const std::string &plugin_type
) {
    std::vector<std::string> param_filters;

    // Plugin filters
    mINI::INIStructure plugin_filter_ini;
    const std::string plugin_filter_path = createPathName({GetPluginCacheFolderPath(), "FilterPlugin.ini"});
    const mINI::INIFile plugin_filter_file(plugin_filter_path);

    if (!std::filesystem::exists(plugin_filter_path)) {
        // File not found, create default
        plugin_filter_ini["global"];
        // Add an example entry so user has an idea of how to add their own plugins
        const std::string default_plugin_section = "developer\\plugin.type";

        plugin_filter_ini[default_plugin_section];
        plugin_filter_ini[default_plugin_section]["enabled"] = "True";
        plugin_filter_ini[default_plugin_section]["override"] = "False";
        plugin_filter_ini[default_plugin_section]["0"] = "MIDI CC";
        plugin_filter_ini[default_plugin_section]["1"] = "MIDI *";
        plugin_filter_ini[default_plugin_section]["2"] = "* CC";
        (void) plugin_filter_file.generate(plugin_filter_ini, true);
    }

    bool plugin_filter_enabled = true;
    bool plugin_filter_override = false;
    const std::string plugin_filter_section_string = developer_name + "\\" + plugin_name + "." + plugin_type;

    // Check plugin filter status
    if (plugin_filter_file.read(plugin_filter_ini)) {
        if (plugin_filter_ini.has(plugin_filter_section_string)) {
            if (plugin_filter_ini[plugin_filter_section_string].has("enabled")) {
                // When this is true, the plugin filter values will be parsed
                plugin_filter_enabled = toBool(plugin_filter_ini[plugin_filter_section_string]["enabled"]);
            }

            if (plugin_filter_enabled) {
                if (plugin_filter_ini[plugin_filter_section_string].has("override")) {
                    // When this is true, FilterDeveloper.ini will be ignored
                    plugin_filter_override = toBool(plugin_filter_ini[plugin_filter_section_string]["override"]);
                }
            }
        } else {
            plugin_filter_enabled = false;
        }
    } else {
        plugin_filter_enabled = false;
    }

    // ------------------------------------------------------------------------

    // Developer filters added first
    if (!plugin_filter_override) {
        mINI::INIStructure developer_filter_ini;
        const std::string developer_filter_path = createPathName({
            GetPluginCacheFolderPath(),
            "FilterDeveloper.ini"
        });

        const mINI::INIFile developer_filter_file(developer_filter_path);
        if (!std::filesystem::exists(developer_filter_path)) {
            // File not found, create default
            developer_filter_ini["global"];
            (void) developer_filter_file.generate(developer_filter_ini, true);
        }

        if (developer_filter_file.read(developer_filter_ini)) {
            if (developer_filter_ini.has(developer_name)) {
                for (int i = 0; i < static_cast<int>(developer_filter_ini[developer_name].size()); i++) {
                    const std::string index = std::to_string(i);

                    if (developer_filter_ini[developer_name].has(index)) {
                        param_filters.push_back(developer_filter_ini[developer_name][index]);
                    }
                }
            }
        }
    }

    // ------------------------------------------------------------------------

    // Plugin filters added second
    if (plugin_filter_enabled) {
        for (int i = 0; i < plugin_filter_ini[plugin_filter_section_string].size(); i++) {
            const std::string index = std::to_string(i);
            if (plugin_filter_ini[plugin_filter_section_string].has(index)) {
                param_filters.push_back(plugin_filter_ini[plugin_filter_section_string][index]);
            }
        }
    }

    return param_filters;
}

mINI::INIMap<std::string> PluginUtils::GetFilterListByDeveloper(const std::string developer) {
    mINI::INIStructure developer_filter_ini;
    const std::string developer_filter_path = createPathName({
        GetPluginCacheFolderPath(),
        "FilterDeveloper.ini"
    });

    const mINI::INIFile developer_filter_file(developer_filter_path);
    if (!developer_filter_file.read(developer_filter_ini)) {
        return {};
    }

    if (!developer_filter_ini.has(developer)) {
        return {};
    }

    return developer_filter_ini[developer];
}

bool PluginUtils::SetFilterListByDeveloper(
    const std::string &developer,
    const std::vector<std::string> &developer_ignore_list
) {
    mINI::INIStructure developer_filter_ini;
    const std::string developer_filter_path = createPathName({
        GetPluginCacheFolderPath(),
        "FilterDeveloper.ini"
    });

    const mINI::INIFile developer_filter_file(developer_filter_path);
    if (!developer_filter_file.read(developer_filter_ini)) {
        return false;
    }

    if (!developer_filter_ini.has(developer)) {
        developer_filter_ini[developer];
    } else {
        developer_filter_ini[developer].clear();
    }

    for (int i = 0; i < static_cast<int>(developer_ignore_list.size()); i++) {
        developer_filter_ini[developer][std::to_string(i)] = developer_ignore_list[i];
    }

    return developer_filter_file.write(developer_filter_ini, true);
}

void PluginUtils::WriteFilterParams(
    const std::vector<std::string> &param_filters,
    const std::vector<int> &param_id,
    const std::vector<std::string> &param_name,
    const std::vector<int> &param_steps,
    mINI::INIStructure &cache
) {
    int param_index = 0;

    for (int i = 0; i < param_id.size(); i++) {
        std::string name = param_name[i];

        // Don't allow empty parameter names
        if (name.empty()) {
            name = "???";
        }

        bool found_filtered_param = false;
        for (const std::string &filter: param_filters) {
            int filtered_length = filter.length();

            // Starts with * (search term at end)
            if (filter.find("*", 0, 1) != std::string::npos) {
                const std::string filtered_param = filter.substr(1, filtered_length - 1);
                filtered_length--;

                if (name.find(filtered_param.c_str(), name.length() - filtered_length, filtered_length) !=
                    std::string::npos) {
                    found_filtered_param = true;
                    break;
                }
            }

            // Ends with * (search term at start)
            filtered_length = filter.length();
            if (filter.find("*", filtered_length - 1, 1) != std::string::npos) {
                const std::string filtered_param = filter.substr(0, filtered_length - 1);
                filtered_length--;

                if (name.find(filtered_param.c_str(), 0, filtered_length) != std::string::npos) {
                    found_filtered_param = true;
                    break;
                }
            }

            // Partial match
            if (name.find(filter) != std::string::npos) {
                found_filtered_param = true;
                break;
            }
        }

        if (!found_filtered_param) {
            cache["id"][std::to_string(param_index)] = std::to_string(param_id[i]);
            cache["params"][std::to_string(param_index)] = name;
            cache["steps"][std::to_string(param_index)] = std::to_string(param_steps[i]);
            param_index++;
        }
    }
}
