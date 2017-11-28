//
// Created by Oleg on 11/25/17.
//

#include "Utils.h"

using namespace utils;

LoggingGuard& LoggingGuard::singleton(const std::string& log_filename,
                                      LogLevel log_level)
{
    thread_local LoggingGuard guard(log_filename, log_level);
    return guard;
}

LoggingGuard::~LoggingGuard() noexcept
{
    if (m_owns)
    {
        ReleaseLogging();
    }
}

LoggingGuard::LoggingGuard(LoggingGuard&& other) noexcept
        : m_owns(other.m_owns)
{
    other.m_owns = false;
}

LoggingGuard& LoggingGuard::operator=(LoggingGuard&& other) noexcept
{
    LoggingGuard temp(std::move(other));
    using std::swap;
    swap(m_owns, temp.m_owns);
    return *this;
}

LoggingGuard::LoggingGuard(const std::string& log_filename,
                           LogLevel log_level)
        : m_owns(false)
{
    if (!InitLogging(log_filename.c_str(), log_level))
    {
        throw std::runtime_error("Failed to initialize logging");
    }
    m_owns = true;
}

stop_evolution_error::stop_evolution_error()
        : std::logic_error("Evolution has been asked to stop")
{
}
