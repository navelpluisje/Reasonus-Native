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

#define LOG_LEVEL 3 // 1=ERROR, 2=WARNING, 3=INFO, 4=DEBUG, 5=TRACE

#if LOG_LEVEL >= 1
#define LOG_ERROR(msg, ...) { CSurf_Log::GetInstance().Write("ERROR", (msg), ##__VA_ARGS__); }
#else
#define LOG_ERROR(msg, ...) {}
#endif

#if LOG_LEVEL >= 2
#define LOG_WARNING(msg, ...) { CSurf_Log::GetInstance().Write("WARNING", (msg), ##__VA_ARGS__); }
#else
#define LOG_WARNING(msg, ...) {}
#endif

#if LOG_LEVEL >= 3
#define LOG_INFO(msg, ...) { CSurf_Log::GetInstance().Write("INFO", (msg), ##__VA_ARGS__); }
#else
#define LOG_INFO(msg, ...) {}
#endif

#if LOG_LEVEL >= 4
#define LOG_DEBUG(msg, ...) { CSurf_Log::GetInstance().Write("DEBUG", (msg), ##__VA_ARGS__); }
#else
#define LOG_DEBUG(msg, ...) {}
#endif

#if LOG_LEVEL >= 5
#define LOG_TRACE(msg, ...) { CSurf_Log::GetInstance().Write("TRACE", (msg), ##__VA_ARGS__); }
#else
#define LOG_TRACE(msg, ...) {}
#endif

#endif