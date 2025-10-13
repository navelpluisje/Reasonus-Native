#ifndef _SCURF_OSARA_H_
#define _SCURF_OSARA_H_

#include <string>

class CSurfOsara
{
    void (*osara_outputMessage)(const char *message); // global
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