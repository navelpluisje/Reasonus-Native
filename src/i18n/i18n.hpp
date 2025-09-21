#ifndef I18N_HPP_
#define I18N_HPP_

#include <string>

class I18n
{
protected:
    static std::string language;

public:
    static void SetLanguage(std::string language);
    static std::string t(std::string key);
};

#endif