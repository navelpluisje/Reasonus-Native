#ifndef I18N_HPP_
#define I18N_HPP_

#include <string>
#include <map>
// #include "../i18n/i18n_locales_enUS.hpp"
// #include "../i18n/i18n_locales_deDE.hpp"

enum Languages
{
    LANG_ENUS,
    LANG_DEDE,
};

class I18n
{
private:
    // Private Constructor
    I18n() {};

    Languages language;
    std::map<std::string, std::string> translations;
    static I18n *instancePtr;

public:
    I18n(const I18n &obj) = delete;

    static I18n *GetInstance()
    {
        if (instancePtr == nullptr)
        {
            instancePtr = new I18n();
        }
        return instancePtr;
    }

    void SetLanguage(Languages lang);
    // {
    //     this->language = lang;
    //     switch (lang)
    //     {
    //     case LANG_ENUS:
    //         this->translations = enUS;
    //         break;

    //     case LANG_DEDE:
    //         this->translations = deDE;
    //         break;

    //     default:
    //         this->translations = enUS;
    //     }
    // }

    std::string t(std::string key);
    // {
    //     if (this->translations.find(key) == this->translations.end())
    //     {
    //         return key;
    //     }
    //     return this->translations[key];
    // }
};

#endif