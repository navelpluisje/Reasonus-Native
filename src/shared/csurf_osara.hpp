#ifndef _SCURF_OSARA_H_
#define _SCURF_OSARA_H_

#include <string>

// void (*osara_outputMessage)(const char *);

// static void import_osara_api()
// {
//     plugin_register("-timer", (void *)&import_osara_api);
//     osara_outputMessage = (decltype(osara_outputMessage))plugin_getapi("osara_outputMessage");
// }

class CSurfOsara
{
    int test;

public:
    // Deleted copy constructor and assignment operator
    CSurfOsara(const CSurfOsara &) = delete;
    CSurfOsara &operator=(const CSurfOsara &) = delete;

    // Public method to access the instance
    static CSurfOsara &GetInstance();

    void Speak(std::string message);

private:
    CSurfOsara();
};

#endif