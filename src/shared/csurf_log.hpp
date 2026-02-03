#ifndef _CSURF_LOG_H
#define _CSURF_LOG_H

#include <string>
#include <cstdarg>

// Singleton logging object
class CSurf_Log
{
private:
    std::string m_filename = "";

    CSurf_Log();

    // Delete copy, and move constructors
    CSurf_Log(const CSurf_Log&) = delete;
    CSurf_Log& operator=(const CSurf_Log&) = delete;
    CSurf_Log(CSurf_Log&&) = delete;
    CSurf_Log& operator=(CSurf_Log&&) = delete;

public:
    static CSurf_Log& GetInstance()
    {
        static CSurf_Log log;
        return log;
    }

    void Write(const std::string &_level, const char *_fmt, ...);
};

#if 0 // 1=Logging enabled, 0=Logging disabled
#define LOG(msg, ...) { CSurf_Log::GetInstance().Write("INFO", (msg), ##__VA_ARGS__); }
#else
#define LOG(msg, ...) {}
#endif

#endif