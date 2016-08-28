//
// Created by Oleg on 5/28/16.
//

#ifndef EVOLUTION_LOGGING_H
#define EVOLUTION_LOGGING_H

#define LOG_FUNC_START(func) Log(INFO, func ": start")
#define LOG_FUNC_END(func) Log(INFO, func ": success")

#ifndef NDEBUG
    #include <assert.h>
    #define LOG_ASSERT(cond) if (!(cond)) { Log(ERROR, "Assertion error: %s : %s : %d", __FILE__, __func__, __LINE__); assert(cond); }
#else
    #define LOG_ASSERT(cond)
#endif

#include <signal.h>
#define LOG_RELEASE_ASSERT(cond) if (!(cond)) { Log(ERROR, "Assertion error: %s : %s : %d", __FILE__, __func__, __LINE__); raise(SIGABRT); }

typedef enum {NOT_SET, INFO, DEBUG, ERROR} LogLevel;

int InitLogging(const char* log_filename, LogLevel log_level);

void ReleaseLogging();

LogLevel SetLogLevel(LogLevel log_level);

void Log(LogLevel log_level, const char* format, ...);

void LogMaxFitness(double fitness);

#endif //EVOLUTION_LOGGING_H
