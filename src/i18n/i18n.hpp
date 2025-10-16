#ifndef I18N_HPP_
#define I18N_HPP_

#include <string>
#include <map>

enum Languages
{
    LANG_ENUS,
    LANG_DEDE,
};

class I18n
{
protected:
    static std::string language;
    static std::map<std::string, std::string> translations;

public:
    static void SetLanguage(Languages language);
    static std::string t(std::string key);
};

#endif