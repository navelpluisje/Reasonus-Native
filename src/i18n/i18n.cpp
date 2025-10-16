#include "../i18n/i18n.hpp"
#include "../i18n/i18n_locales_enUS.hpp"
#include "../i18n/i18n_locales_deDE.hpp"

void I18n::SetLanguage(Languages lang)
{
    language = lang;
    switch (lang)
    {
    case LANG_ENUS:
        translations = enUS;
        break;

    case LANG_DEDE:
        translations = deDE;
        break;

    default:
        translations = enUS;
    }
}

std::string I18n::t(std::string key)
{
    if (translations.find(key) == translations.end())
    {
        return key;
    }
    return translations[key];
}