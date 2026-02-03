#ifndef _CSURF_LOG_H
#define _CSURF_LOG_H

#include <string>
#include <cstdarg>

// Singleton logging object
class CSurf_Log
{
private:
    std::string m_filename = "";
    int m_logLevel = 3;

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

    void Write(int _level, const char *_fmt, ...);
};

#define LOG_ERROR(msg, ...) { CSurf_Log::GetInstance().Write(1, (msg), ##__VA_ARGS__); }
#define LOG_WARNING(msg, ...) { CSurf_Log::GetInstance().Write(2, (msg), ##__VA_ARGS__); }
#define LOG_INFO(msg, ...) { CSurf_Log::GetInstance().Write(3, (msg), ##__VA_ARGS__); }
#define LOG_DEBUG(msg, ...) { CSurf_Log::GetInstance().Write(4, (msg), ##__VA_ARGS__); }
#define LOG_TRACE(msg, ...) { CSurf_Log::GetInstance().Write(5, (msg), ##__VA_ARGS__); }

#endif