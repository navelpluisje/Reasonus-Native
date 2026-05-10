#include "csurf_plugin_utils.hpp"

#include <regex>
#include <WDL/wdltypes.h> // might be unnecessary in future

#include "csurf_daw.hpp"
#include "reaper_plugin_functions.h"
#include "csurf_utils.hpp"


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
    std::vector<std::string> plugin_name_parts = split(
        StripPluginNamePrefixes(StripPluginChannelPostfix(plugin_name.c_str()).data()),
        " ("
    );

    if (plugin_name_parts.size() > 1) {
        plugin_name_parts.pop_back();
    }

    return join(plugin_name_parts, " (");
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

std::vector<PluginMeta> PluginUtils::ExtractInstalledPluginMeta(std::set<std::string> &developers) {
    const std::vector<std::string> installed_plugins = GetInstalledPlugins();
    std::vector<PluginMeta> plugin_meta;
    mINI::INIStructure category_file;
    const mINI::INIFile file(createPathName({GetResourcePath(), "reaper-fxtags.ini"}));
    file.read(category_file);

    for (const auto &plugin_name: installed_plugins) {
        PluginMeta meta(plugin_name);
        const std::string developer = ExtractDeveloperName(plugin_name);
        const std::string plugin_type = ExtractPluginType(plugin_name);
        const std::string cat_name = createCategoryName(plugin_name, plugin_type);

        developers.emplace(developer);
        meta.SetDeveloper(developer);
        meta.SetFullName(plugin_name);
        meta.SetCategory(category_file.get("category").get(cat_name));
        meta.SetPluginType(toLowerCase(plugin_type));

        plugin_meta.emplace_back(meta);
    }

    return plugin_meta;
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

std::string PluginUtils::FormatPluginType(std::string value) {
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

bool PluginUtils::CreatePluginMappingCacheFile(MediaTrack *media_track, int plugin_id, bool update) {
    char plugin_name[256] = ""; // NOLINT(*-avoid-c-arrays)
    int param_index = 0;
    mINI::INIStructure cache;

    if (!TrackFX_GetFXName(media_track, plugin_id, plugin_name, std::size(plugin_name))) {
        return false;
    }

    const int nb_params = TrackFX_GetNumParams(media_track, plugin_id);
    const std::string developer = ExtractDeveloperName(plugin_name);
    const std::string short_name = ExtractPluginName(plugin_name);
    const std::string plugin_type = ExtractPluginType(plugin_name);
    const std::string cache_path = GetReaSonusPluginCachePath(developer, short_name, plugin_type, true);

    cache["Global"];
    cache["Global"]["origName"] = StripPluginNamePrefixes(plugin_name);
    cache["Global"]["name"] = short_name;
    cache["Global"]["type"] = plugin_type;
    cache["Global"]["developer"] = developer;

    for (int i = 0; i < nb_params; i++) {
        const std::string param_name = DAW::GetTrackFxParamName(media_track, 0, i);

        if (IsWantedParam(std::string(param_name))) {
            const std::string param_idx = "param_" + std::to_string(param_index);

            cache[param_idx];
            cache[param_idx]["id"] = std::to_string(i);
            cache[param_idx]["name"] = DAW::GetTrackFxParamName(media_track, plugin_id, i);
            cache[param_idx]["steps"] = std::to_string(DAW::GetTrackFxParamNbSteps(media_track, plugin_id, i));

            param_index++;
        }
    }

    cache["Global"]["nb_params"] = std::to_string(param_index - 1);


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
