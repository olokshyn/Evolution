//
// Created by Oleg on 11/25/17.
//

#ifndef GUI_UTILS_H
#define GUI_UTILS_H

#include <string>

extern "C"
{
#include "Logging/Logging.h"
}

namespace utils
{
    class LoggingGuard
    {
    public:
        static LoggingGuard& singleton(const std::string& log_filename,
                                       LogLevel log_level);

    public:
        ~LoggingGuard() noexcept;

        LoggingGuard(LoggingGuard&& other) noexcept;
        LoggingGuard& operator=(LoggingGuard&& other) noexcept;

    private:
        explicit LoggingGuard(const std::string& log_filename,
                              LogLevel log_level);

    private:
        bool m_owns;
    };
}

#endif //GUI_UTILS_H
