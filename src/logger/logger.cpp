#include "logger.h"
#include <iostream>
#include <algorithm>

namespace logger {

Logger::Logger(const std::string& filename, Level defaultLevel)
    : filename_(filename)
    , defaultLevel_(defaultLevel)
    , stopFlag_(false)
{
    worker_ = std::thread(&Logger::workerThread, this);
}

Logger::~Logger()
{
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        stopFlag_ = true;
    }
    condition_.notify_one();
    if (worker_.joinable()) {
        worker_.join();
    }
}

void Logger::log(const std::string& message, Level level)
{
    if (level < defaultLevel_) {
        return;
    }

    LogEntry entry;
    entry.message = message;
    entry.level = level;
    entry.timestamp = std::chrono::system_clock::now();

    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        queue_.push(entry);
    }
    condition_.notify_one();
}

void Logger::setDefaultLevel(Level level)
{
    defaultLevel_ = level;
}

Level Logger::getDefaultLevel() const
{
    return defaultLevel_;
}

void Logger::workerThread()
{
    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex_);
        condition_.wait(lock, [this]() {
            return !queue_.empty() || stopFlag_;
        });

        if (stopFlag_ && queue_.empty()) {
            break;
        }

        if (!queue_.empty()) {
            LogEntry entry = queue_.front();
            queue_.pop();
            lock.unlock();
            writeLogEntry(entry);
        }
    }
}

void Logger::writeLogEntry(const LogEntry& entry)
{
    try {
        std::ofstream file(filename_, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "Error: Failed to open log file: " << filename_ << std::endl;
            return;
        }

        file << formatTimestamp(entry.timestamp)
             << " [" << levelToString(entry.level) << "] "
             << entry.message << std::endl;

        if (file.fail()) {
            std::cerr << "Error: Failed to write to log file: " << filename_ << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: Exception while writing log: " << e.what() << std::endl;
    }
}

std::string Logger::formatTimestamp(const std::chrono::system_clock::time_point& tp) const
{
    auto timeT = std::chrono::system_clock::to_time_t(tp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        tp.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&timeT), "[%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count() << "]";
    return ss.str();
}

std::string Logger::levelToString(Level level) const
{
    switch (level) {
        case Level::INFO:    return "INFO";
        case Level::WARNING: return "WARNING";
        case Level::ERROR:   return "ERROR";
        default:             return "UNKNOWN";
    }
}

} // namespace logger