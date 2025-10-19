#include "../osara/osara.hpp"
#include "../osara/osara_locales_enUS.hpp"
#include "../osara/osara_locales_deDE.hpp"
#include "../shared/csurf_utils.hpp"
#include "../controls/csurf_fader_resources.hpp"

void Osara::SetLanguage(OsaraLanguages lang)
{
    this->language = lang;
    switch (lang)
    {
    case OSARA_LANG_ENUS:
        this->translations = osaraEnUS;
        break;

    case OSARA_LANG_DEDE:
        this->translations = osaraDeDE;
        break;

    default:
        this->translations = osaraEnUS;
    }
}

void Osara::SetLanguage(int lang)
{
    SetLanguage((OsaraLanguages)lang);
}

void Osara::Speak(std::string message)
{
    osara_outputMessage(message.c_str());
}

void Osara::Training(int message, int control, int value)
{
    osara_outputMessage(CreateTrainingKey(message, control, value).c_str());
}

std::string Osara::CreateTrainingKey(int message, int control, int value)
{
    if (message >= FADER_1 && message <= FADER_16)
    {
        return "Track mode Fader Channel " + std::to_string((FADER_16 - FADER_1) + 1);
    }
    return "Bliep";
}