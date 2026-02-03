#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <locale>
#include "csurf_utils.hpp"
#include "csurf_log.hpp"

CSurf_Log::CSurf_Log()
{
    m_filename = createPathName({GetReaSonusFolderPath(), "ReaSonus.log"});

    // Clear the log file
    FILE* f = fopen(m_filename.c_str(), "w");
    fclose(f);

    Write("INFO", "CSurf_Log: Started log file at %s", m_filename.c_str());
}

void CSurf_Log::Write(const std::string &_level, const char *_fmt, ...)
{
    // Open and close the log file for each write, to make sure the log persists
    // in case of a crash
    FILE* f = fopen(m_filename.c_str(), "a");
    auto t = std::time(nullptr);
    char timestamp[24];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

    va_list(args);
    fprintf(f, "%19s %7s: ", timestamp, _level.c_str());
    va_start(args, _fmt);
    vfprintf(f, _fmt, args);
    va_end(args);
    fprintf(f, "\n");
    fclose(f);
}
