#ifndef CSURF_PLUGIN_UTILS_H_
#define CSURF_PLUGIN_UTILS_H_

#include <string>
#include <vector>
#include <set>

#include "csurf_utils.hpp"
#include "reaper_plugin_functions.h"

class MediaTrack;

struct PluginMeta {
private:
    std::string developer;
    std::string category;
    std::string full_name;
    std::string plugin_type;

public:
    explicit PluginMeta(const std::string &_full_name) {
        full_name = _full_name;
    }

    [[nodiscard]] std::string GetDeveloper() const {
        return developer;
    }

    void SetDeveloper(const std::string &_developer) {
        developer = _developer;
    }

    [[nodiscard]] std::string GetCategory() const {
        return category;
    }

    void SetCategory(const std::string &_category) {
        category = _category;
    }

    [[nodiscard]] std::string GetFullName() const {
        return full_name;
    }

    void SetFullName(const std::string &_full_name) {
        full_name = _full_name;
    }

    [[nodiscard]] std::string GetPluginType() const {
        return plugin_type;
    }

    void SetPluginType(const std::string &_plugin_type) {
        plugin_type = _plugin_type;
    }
};

class PluginUtils {
public:
    /**
     * Get the folder path for the reasonus plugins
     * @return
     */
    static std::string GetPluginFolderPath();

    /**
     * Get the folder path for the reasonus plugin cache files
     * @return
     */
    static std::string GetPluginCacheFolderPath();

    /**
     * Get the patgh to the plugin file with the given parguments
     * @param developer The developer folder
     * @param plugin_name The plugin name to use for creating the file nmame
     * @param plugin_type The plugin type to use for creating the file nmame
     * @param create Wether to create the path if it not exists
     * @return
     */
    static std::string GetReaSonusPluginPath(
        std::string developer,
        const std::string &plugin_name,
        const std::string &plugin_type,
        bool create
    );

    /**
     * Get the patgh to the plugin file with the given parguments
     * @param developer The developer folder
     * @param plugin_name The plugin name to use for creating the file nmame
     * @param plugin_type The plugin type to use for creating the file nmame
     * @param create Wether to create the path if it not exists
     * @return
     */
    static std::string GetReaSonusPluginCachePath(
        std::string developer,
        const std::string &plugin_name,
        const std::string &plugin_type,
        bool create
    );

    /**
     * @brief Strip the plugin name from its pre- and post-fixes
     *
     * @param plugin_name The full plugin name
     * @return The stripped down name
     */
    static std::string StripPluginName(const std::string &plugin_name);

    /**
     * @brief Strip the plugin developer from its pre- and post-fixes
     *
     * @param plugin_name The full plugin name
     * @return The stripped down name
     */
    static std::string StripPluginDeveloper(const std::string &plugin_name);

    /**
     * @brief Strip the type from the plugin name
     *
     * @param name The full plugin name
     * @return std::string
     */
    static std::string StripPluginNamePrefixes(char const *name);

    /**
     * @brief Strip the final out info when applicable
     *
     * @param name
     * @return std::string
     */
    static std::string StripPluginChannelPostfix(char const *name);

    /**
     * Extract the developer name from REAPER's plugin name
     * @param plugin_name The name of the plugin to extract the developer name from
     * @return The developer name
     */
    static std::string ExtractDeveloperName(const std::string &plugin_name);

    /**
     * Extract the plugin name from REAPER's plugin name
     * @param plugin_name The name of the plugin to extract the developer name from
     * @return The plugin name
     */
    static std::string ExtractPluginName(const std::string &plugin_name);

    /**
     * Extract the plugin type from REAPer's plugin name
     * @param plugin_name The name of the plugin to extract the type name from
     * @return The plugin type
     */
    static std::string ExtractPluginType(const std::string &plugin_name);

    /**
     * Get the path of the folder where ReaSonus is storing all the plugin mappings
     * @return
     */
    static std::string GetPluginsPath();

    /**
     * Check if the plugin has a mapping file
     * @param media_track The media track where th eplugin is added
     * @param pluginId The plugin index
     * @return Wether the plugin has a mapping file
     */
    static bool hasPluginConfigFile(MediaTrack *media_track, int pluginId);

    /**
     * @brief Check if the prefix of the fx tells it is actually a fx and not an instrument
     *
     * @param plugin_name
     * @return bool
     */
    static bool IsPluginFX(std::string plugin_name);


    /**
     * Get a list of all the developers with mapped plugins
     * @param sorted Whether to sort the result
     * @return
     */
    static std::vector<std::string> GetpluginDevelopers(bool sorted);

    /**
     * Get a list of all the mapped plugins by developer
     * @param sorted Whether to sort the result
     * @return
     */
    static std::vector<std::string> GetDeveloperPlugins(std::string developer, bool sorted);

    /**
     * Get a list with all the installed plugins according to REAPER
     * @return
     */
    static std::vector<std::string> GetInstalledPlugins();

    /**
     * Create a composed plugin name as it is used for storing the categories in REAPER
     * @param plugin_name The stripped name of the plugin
     * @param plugin_type The extension of the plugin
     * @return
     */
    static std::string createCategoryName(const std::string &plugin_name, std::string plugin_type);

    /**
     * Create a list with objects containing meta data per plugin which can be used for plugin selection
     * @param developers
     * @return
     */
    static std::vector<PluginMeta> ExtractInstalledPluginMeta(std::set<std::string> &developers);

    /**
     * Fetch a list of plugin types allowed in the plugin mapping
     * @return
     */
    static std::vector<std::string> GetPluginTypes();

    /**
     * Convert the plugin type to uppercase and if it's an instrument, make sure the `i` is lowercase
     * @param value Plugin type to format
     * @return
     */
    static std::string FormatPluginType(std::string value);

    /**
     * Check the plugin is one of the allowed types and prefix the "name (developer)" string with the type
     *
     * Also change VST to VST2, assuming it would be v2.x and not the older v1.x which is obsolete
     * @param plugin_origname Original name of the plugin from TrackFX_GetFXName
     * @param plugin_type Identifier to determine which plugin package to load
     * @return
     */
    static std::string GetPluginRequestString(const std::string &plugin_origname, std::string plugin_type);

    static bool CreatePluginMappingCacheFile(MediaTrack *media_track, int plugin_id, bool update);

    static bool UpdatePluginMappingCacheFile(const std::string &full_plugin_name);

    static mINI::INIStructure GetPluginMappingCacheStructure(
        const std::string &developer,
        const std::string &plugin_name,
        const std::string &plugin_type
    );

    static bool HasPluginMappingCache(
        const std::string &developer,
        const std::string &plugin_name,
        const std::string &plugin_type
    );

    static bool HasPluginMappingCache(
        const std::string &cache_path
    );
};

#endif
