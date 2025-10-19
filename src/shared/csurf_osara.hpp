#ifndef CSURF_OSARA_HPP_
#define CSURF_OSARA_HPP_

#include <string>
#include <map>
// #include "../i18n/i18n_locales_enUS.hpp"
// #include "../i18n/i18n_locales_deDE.hpp"

class CSurfOsara
{
private:
    // Private Constructor
    CSurfOsara() {};

    // Languages language;
    std::map<std::string, std::string> translations;
    static CSurfOsara *instancePtr;

public:
    CSurfOsara(const CSurfOsara &obj) = delete;
    void (*osara_outputMessage)(const char *);

    static CSurfOsara *GetInstance()
    {
        if (instancePtr == nullptr)
        {
            instancePtr = new CSurfOsara();
        }
        return instancePtr;
    }

    void Speak(std::string value);

    // void SetLanguage(Languages lang);
    // void SetLanguage(int lang);
};

#endif