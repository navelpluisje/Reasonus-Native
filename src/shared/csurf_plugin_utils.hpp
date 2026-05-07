#ifndef CSURF_PLUGIN_UTILS_H_
#define CSURF_PLUGIN_UTILS_H_

#include <string>
#include <vector>
#include <set>

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

    static std::string ExtractDeveloperName(const std::string &plugin_name);

    static std::string ExtractPluginType(const std::string &plugin_name);

    static std::string GetPluginsPath();

    static std::vector<std::string> GetpluginDevelopers(bool sorted);

    static std::vector<std::string> GetDeveloperPlugins(std::string developer, bool sorted);

    static std::vector<std::string> GetInstalledPlugins();

    static std::string createCategoryname(std::string plugin_name, std::string plugin_type);

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
};

#endif
