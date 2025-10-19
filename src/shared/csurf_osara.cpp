#include "./csurf_osara.hpp"
#include "../shared/csurf_utils.hpp"

void CSurfOsara::SetLanguage(Languages lang)
{
    this->language = lang;
    // switch (lang)
    // {
    // case LANG_ENUS:
    //     this->translations = enUS;
    //     break;

    // case LANG_DEDE:
    //     this->translations = deDE;
    //     break;

    // default:
    //     this->translations = enUS;
    // }
}

void CSurfOsara::SetLanguage(int lang)
{
    SetLanguage((Languages)lang);
}

void CSurfOsara::Speak(std::string value)
{
    this->osara_outputMessage(value.c_str());
}