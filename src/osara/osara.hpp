#ifndef OSARA_HPP_
#define OSARA_HPP_

#include <string>
#include <map>
#include <WDL/wdltypes.h> // might be unnecessary in future
#include <reaper_plugin_functions.h>

// #include "../i18n/i18n_locales_enUS.hpp"
// #include "../i18n/i18n_locales_deDE.hpp"

enum OsaraLanguages
{
    OSARA_LANG_ENUS,
    OSARA_LANG_DEDE,
};

class Osara
{
private:
    // Private Constructor
    Osara() {};

    OsaraLanguages language;
    std::map<std::string, std::string> translations;
    static Osara *instancePtr;

public:
    Osara(const Osara &obj) = delete;
    void (*osara_outputMessage)(const char *);

    static Osara *GetInstance()
    {
        if (instancePtr == nullptr)
        {
            instancePtr = new Osara();
        }
        return instancePtr;
    }

    static void ImportOsaraApi()
    {
        plugin_register("-timer", (void *)&Osara::ImportOsaraApi);
        Osara::GetInstance()->osara_outputMessage = (decltype(Osara::osara_outputMessage))plugin_getapi("osara_outputMessage");
    }

    void SetLanguage(OsaraLanguages lang);
    void SetLanguage(int lang);

    void Speak(std::string message);

    void Training(int message, int control, int value);

    std::string CreateTrainingKey(int message, int control, int value);
};

#endif