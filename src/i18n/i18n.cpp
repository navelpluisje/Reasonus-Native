#include "../i18n/i18n.hpp"
#include "../i18n/i18n_locales_enUS.hpp"
#include "../i18n/i18n_locales_deDE.hpp"
#include "../shared/csurf_utils.hpp"

void I18n::SetLanguage(Languages lang)
{
    this->language = lang;
    switch (lang)
    {
    case LANG_ENUS:
        this->translations = enUS;
        break;

    case LANG_DEDE:
        this->translations = deDE;
        break;

    default:
        this->translations = enUS;
    }
}

void I18n::SetLanguage(int lang)
{
    SetLanguage((Languages)lang);
}

std::string I18n::t(std::string key)
{
    if (this->translations.find(key) == this->translations.end())
    {
        return key;
    }
    return this->translations[key];
}

std::string I18n::t(std::string key, std::string arg1)
{
    char buffer[512];
    std::string translation = t(key);
    std::snprintf(buffer, 512, translation.c_str(), arg1.c_str());

    std::string result = buffer;
    return result;
}

std::string I18n::t(std::string key, std::string arg1, std::string arg2)
{
    char buffer[512];
    std::string translation = t(key);
    std::snprintf(buffer, 512, translation.c_str(), arg1.c_str(), arg2.c_str());

    std::string result = buffer;
    return result;
}

std::string I18n::t(std::string key, std::string arg1, std::string arg2, std::string arg3)
{
    char buffer[512];
    std::string translation = t(key);
    std::snprintf(buffer, 512, translation.c_str(), arg1.c_str(), arg2.c_str(), arg3.c_str());

    std::string result = buffer;
    return result;
}