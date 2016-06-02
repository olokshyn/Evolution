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


static const char* GetLogLevelName(LogLevel log_level);

int InitLogging(const char* log_filename, LogLevel log_level) {
    if (!log_file) {
        ReleaseLogging();
    }
    log_file = fopen(log_filename, "w");
    if (!log_file) {
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
        log_file = NULL;
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
    fprintf(log_file, "[%s]:", GetLogLevelName(log_level));
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


// static functions -------


static const char* GetLogLevelName(LogLevel log_level) {
    static const char* info = "INFO";
    static const char* debug = "DEBUG";
    static const char* error = "ERROR";
    static const char* no_level = "Log level is not set";
    switch (log_level) {
        case INFO:
            return info;
        case DEBUG:
            return debug;
        case ERROR:
            return error;
        default:
            return no_level;
    }
}