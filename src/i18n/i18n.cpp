#include "../i18n/i18n.hpp"
#include "../i18n/i18n_locales_enUS.hpp"
#include "../i18n/i18n_locales_deDE.hpp"

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

std::string I18n::t(std::string key)
{
    if (this->translations.find(key) == this->translations.end())
    {
        return key;
    }
    return this->translations[key];
}