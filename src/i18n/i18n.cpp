#include <mini/ini.h>
#include "../i18n/i18n.hpp"
#include "../i18n/i18n_locales_enUS.hpp"
#include "../i18n/i18n_locales_deDE.hpp"
#include "../shared/csurf_utils.hpp"

I18n::I18n() {}

void I18n::SetLanguage(std::string lang)
{
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
        file.read(translations);
    }
}

void I18n::SetLanguage(int lang)
{
    SetLanguage((Languages)lang);
}

std::string I18n::t(std::string group, std::string key)
{
    if (!this->translations.has(group) || !this->translations[group].has(key))
    {
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