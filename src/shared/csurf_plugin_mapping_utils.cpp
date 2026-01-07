#include <string>
#include <vector>
#include <format>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>
#include "csurf_utils.hpp"
#include "csurf_database.hpp"
#include "csurf_daw.hpp"

std::string EscapeValue(std::string value)
{
    std::vector<std::string> tmp = split(value, "'");
    return join(tmp, "''");
}

std::string DeserializeValue(std::string value)
{
    std::vector<std::string> tmp = split(value, "''");
    return join(tmp, "'");
}

std::string GetReaperPluginsFile()
{
    bool has_next = true;
    int index = 0;
    std::string pluginFilePath;

    while (has_next)
    {
        const char *name = ::EnumerateFiles(::GetResourcePath(), index);
        if (!name)
        {
            has_next = false;
        }
        else
        {
            index++;
            if (startsWith(name, "reaper-vstplugins_"))
            {
                pluginFilePath = std::string(::GetResourcePath()) + pathSeparator + std::string(name);
            }
        }
    }

    return pluginFilePath;
}

void PopulatePluginsDatabase(Database *db)
{
    std::string filePath = GetReaperPluginsFile();
    if (filePath == "")
    {
        // Show a message that we were not able to find the correct file
        return;
    }

    mINI::INIStructure plugins;
    mINI::INIFile file(filePath);
    if (!file.read(plugins))
    {
        // Show a message that we were not able to find the correct file
        return;
    }

    for (auto const &plugin : plugins.get("vstcache"))
    {
        auto const &pluginName = plugin.first;
        const std::string &value = plugin.second;
        std::vector<std::string> items = split(value, ",");
        if (items.size() < 3 || items[2] == "<SHELL>")
        {
            continue;
        }
        std::vector<std::string> splitted_name = split(items[2], "!!!");
        std::string name = StripPluginName(splitted_name[0]);
        std::string developer = StripPluginDeveloper(splitted_name[0]);
        std::string instrument = (splitted_name.size() > 1) ? "1" : "0";
        std::string pluginId = split(items[1], "{")[0];
        std::string pluginType = toUpper(split(split(pluginName, ".").back(), "<")[0]);

        db->execute(
            "INSERT INTO plugins (name, developer, type, pluginId, originalName, reaperName, timestamp, instrument) VALUES ('" +
            EscapeValue(name) + "', '" +
            developer + "', '" +
            pluginType + "', '" +
            pluginId + "', '" +
            EscapeValue(items[2]) + "', '" +
            EscapeValue(pluginType + ((instrument == "0") ? ": " : "i: ") + splitted_name[0]) + "', '" +
            items[0] + "', " +
            instrument + ");");
    }
}

void PopulatePluginParameters(Database *db)
{
    // Create a track, add the plugin and start reading the params
    InsertTrackAtIndex(0, false);
    MediaTrack *media_track = GetTrack(0, 0);

    auto plugins = db->FetchPlugins();
    for (const auto &[id, name, pluginId] : plugins)
    {
        media_track = GetTrack(0, 0);

        int x;

        try
        {
            x = TrackFX_AddByName(media_track, name.c_str(), false, -1);
        }
        catch (...)
        {
            continue;
        }
        logInteger("x", x);

        for (int j = 0; j < std::min(TrackFX_GetNumParams(media_track, x), 4); j++)
        {
            std::string param_name = DAW::GetTrackFxParamName(media_track, x, j);
            ShowConsoleMsg(param_name.c_str());
            // if (IsWantedParam(std::string(param_name)))
            // {
            //     int steps = DAW::GetTrackFxParamNbSteps(media_track, 0, i);
            //     params.push_back(std::string(param_name));
            //     paramIds.push_back({i, std::string(param_name), steps});
            // }
        }
        TrackFX_Delete(media_track, 0);

        ShowConsoleMsg((std::to_string(id) + " " + name + " " + pluginId + "\n").c_str());
    }
}
