#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <locale>
#include <filesystem>
#include "csurf_utils.hpp"
#include "csurf_log.hpp"

CSurf_Log::CSurf_Log()
{
    std::string reasonusDirectory = GetReaSonusFolderPath();
    std::string logsDirectory = createPathName({reasonusDirectory, "Logs"});
    m_filename = createPathName({logsDirectory, "ReaSonus.log"});

    // Create directory structure, if it does not exist already
    if (!std::filesystem::exists(reasonusDirectory))
    {
        std::filesystem::create_directory(reasonusDirectory);
    }

    if (!std::filesystem::exists(logsDirectory))
    {
        std::filesystem::create_directory(logsDirectory);
    }

    // Clear the log file
    FILE* fileHandle = fopen(m_filename.c_str(), "w");
    fclose(fileHandle);

    // Read the log level from the REASONUS_LOG_LEVEL enviroment variable, if it exists
    const char* envVarLogLevel = std::getenv("REASONUS_LOG_LEVEL");
    if (envVarLogLevel != nullptr)
    {
        m_logLevel = std::atoi(envVarLogLevel);
    }
}

void CSurf_Log::Write(int _level, const char *_fmt, ...)
{
    if (_level > m_logLevel)
    {
        // Log message filtered out
        return;
    }

    // Open and close the log file for each write, to make sure the log persists
    // in case of a crash
    FILE* fileHandle = fopen(m_filename.c_str(), "a");
    auto t = std::time(nullptr);
    char timestamp[24];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

    // Convert log level to a string
    const char* levelString;
    switch (_level)
    {
    case 1:
        levelString = "ERROR";
        break;
    case 2:
        levelString = "WARNING";
        break;
    case 3:
        levelString = "INFO";
        break;
    case 4:
        levelString = "DEBUG";
        break;
    case 5:
        levelString = "TRACE";
        break;
    default:
        levelString = "UNKNOWN";
        break;
    }

    va_list(args);
    fprintf(fileHandle, "%19s %7s: ", timestamp, levelString);
    va_start(args, _fmt);
    vfprintf(fileHandle, _fmt, args);
    va_end(args);
    fprintf(fileHandle, "\n");
    fclose(fileHandle);
}
