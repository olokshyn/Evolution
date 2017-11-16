//
// Created by Oleg on 5/28/16.
//

#include "Logging.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

static LogLevel current_log_level = INFO;
static FILE* log_file = NULL;
static FILE* fitness_file = NULL;

static const char* const LogLevelsNames[5] = {
        [NOT_SET] = "NOT_SET",
        [DEBUG] = "DEBUG",
        [INFO] = "INFO",
        [WARNING] = "WARNING",
        [ERROR] = "ERROR"
};

int InitLogging(const char* log_filename, LogLevel log_level) {
    if (!log_file) {
        ReleaseLogging();
    }
    log_file = fopen(log_filename, "w");
    if (!log_file) {
        return 0;
    }
    fitness_file = fopen("fitness.log", "w");
    if (!fitness_file) {
        fclose(log_file);
        return 0;
    }
    if (log_level != NOT_SET) {
        current_log_level = log_level;
    }
    return 1;
}

void ReleaseLogging() {
    if (log_file) {
        fclose(log_file);
        fclose(fitness_file);
        log_file = NULL;
        fitness_file = NULL;
    }
}

LogLevel SetLogLevel(LogLevel log_level) {
    LogLevel temp = current_log_level;
    if (log_level != NOT_SET) {
        current_log_level = log_level;
    }
    return temp;
}

void Log(LogLevel log_level, const char* format, ...) {
    if (!log_file || log_level < current_log_level) {
        return;
    }
    fprintf(log_file, "[%s]:", LogLevelsNames[log_level]);
    va_list ap;
    va_start(ap, format);
    char* str = strdup(format);
    char* p = strtok(str, " ");
    while (p) {
        fprintf(log_file, " ");
        if (p[0] == '%') {
            char* temp = p;
            while (!isalpha(*++temp));
            switch (*temp) {
                case 'd':
                    fprintf(log_file, p, va_arg(ap, int));
                    break;
                case 'u':
                    fprintf(log_file, p, va_arg(ap, unsigned int));
                    break;
                case 'z':
                    fprintf(log_file, p, va_arg(ap, size_t));
                    break;
                case 'f':
                    fprintf(log_file, p, va_arg(ap, double));
                    break;
                case 's':
                    fprintf(log_file, p, va_arg(ap, char*));
                    break;
                default:
                    fprintf(log_file, "%s", "Invalid argument");
            }
        }
        else {
            fprintf(log_file, "%s", p);
        }
        p = strtok(NULL, " ");
    }
    fprintf(log_file, "\n");
    fflush(log_file);
    free(str);
    va_end(ap);
}

void LogMaxFitness(double fitness) {
    fprintf(fitness_file, "%.3f\n", fitness);
    fflush(fitness_file);
}