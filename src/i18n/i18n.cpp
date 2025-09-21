#include "../i18n/i18n.hpp"
#include "../i18n/i18n_locales_enUS.hpp"

void I18n::SetLanguage(std::string lang)
{
    language = lang;
}

std::string I18n::t(std::string key)
{
    if (language.compare("de") == 0)
    {
        if (enUS.find(key) == enUS.end())
        {
            return enUS[key];
        }
        return enUS[key];
    }

    return enUS[key];
}