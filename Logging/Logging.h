//
// Created by Oleg on 5/28/16.
//

#ifndef WISHART_LOGGING_H
#define WISHART_LOGGING_H

#define LOG_FUNC_START(func) Log(INFO, func ": start")
#define LOG_FUNC_END(func) Log(INFO, func ": success")

#ifndef NDEBUG
    #include <assert.h>
    #define LOG_ASSERT(cond) if (!(cond)) { Log(ERROR, "Assertion"); assert(cond); }
#else
    #define LOG_ASSERT(cond)
#endif

typedef enum {NOT_SET, INFO, DEBUG, ERROR} LogLevel;

int InitLogging(const char* log_filename, LogLevel log_level);

void ReleaseLogging();

LogLevel SetLogLevel(LogLevel log_level);

void Log(LogLevel log_level, const char* format, ...);

#endif //WISHART_LOGGING_H
