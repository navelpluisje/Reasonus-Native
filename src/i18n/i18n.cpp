#include <filesystem>
#include <config.h>
#include <mini/ini.h>
#include "../i18n/i18n.hpp"
#include "../shared/csurf_utils.hpp"
#include "../shared/csurf_log.hpp"

I18n::I18n() {}

void I18n::SetLanguage(std::string lang)
{
    LOG("I18n::SetLanguage: %s", lang.c_str());
    if (this->language.compare(lang) == 0)
    {
        return;
    }

    this->language = lang;
    std::string locales_path = GetReaSonusLocalesPath(lang);

    mINI::INIFile file(GetReaSonusLocalesPath(lang));
    if (!file.read(translations))
    {
        mINI::INIFile file(GetReaSonusLocalesPath("en-US"));
        if (!file.read(translations))
        {
            LOG("[WARNING] Failed to read translations file!");
            return;
        }
    }
}

std::string I18n::t(std::string group, std::string key)
{
    if (!this->translations.has(group) || !this->translations[group].has(key))
    {
        LOG("[WARNING] I18n::t: Not found! (group=%s, key=%s)", group.c_str(), key.c_str());
        return key;
    }
    return this->translations[group][key];
}

std::string I18n::t(std::string group, std::string key, std::string arg1)
{
    char buffer[512];
    std::string translation = t(group, key);
    std::snprintf(buffer, 512, translation.c_str(), arg1.c_str());

    std::string result = buffer;
    return result;
}

std::string I18n::t(std::string group, std::string key, std::string arg1, std::string arg2)
{
    char buffer[512];
    std::string translation = t(group, key);
    std::snprintf(buffer, 512, translation.c_str(), arg1.c_str(), arg2.c_str());

    std::string result = buffer;
    return result;
}

std::string I18n::t(std::string group, std::string key, std::string arg1, std::string arg2, std::string arg3)
{
    char buffer[512];
    std::string translation = t(group, key);
    std::snprintf(buffer, 512, translation.c_str(), arg1.c_str(), arg2.c_str(), arg3.c_str());

    std::string result = buffer;
    return result;
}

void I18n::checkLocalesFiles()
{
    mINI::INIStructure root_translations;
    mINI::INIStructure translations;

    // copy enUS to ReaSonus/Locales
    std::string locales_root = GetReaSonusLocalesRootFile();
    std::string locales_path = GetReaSonusLocalesPath("en-US");
    try
    {
        const std::filesystem::path dirPath = GetReaSonusLocalesFolderPath();

        if (!std::filesystem::exists(dirPath))
        {
            LOG("I18n::checkLocalesFiles: Create directory: %s", dirPath.c_str());
            std::filesystem::create_directory(dirPath);
        }

        std::filesystem::copy(locales_root, locales_path, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
    }
    catch (std::filesystem::filesystem_error &e)
    {
        LOG("I18n::checkLocalesFiles: File system error: %s", e.what());
        ShowConsoleMsg(e.what());
        return;
    }

    std::vector<std::string> languages;
    GetLanguages(languages);

    for (auto language : languages)
    {
        mINI::INIFile file(GetReaSonusLocalesPath(language));
        file.read(translations);
        mINI::INIFile root(GetReaSonusLocalesPath("en-US"));
        root.read(root_translations);

        for (auto const &sections : translations)
        {
            std::string section = sections.first;
            for (auto const &pair : sections.second)
            {
                auto const &key = pair.first;
                root_translations[section][key] = translations[section][key];
            }
        }

        file.write(root_translations);
    }
}
