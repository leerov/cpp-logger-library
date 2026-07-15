#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace logger {

enum class Level {
    INFO = 0,
    WARNING = 1,
    ERROR = 2
};

class Logger {
public:
    Logger(const std::string& filename, Level defaultLevel);
    ~Logger();

    void log(const std::string& message, Level level);
    void setDefaultLevel(Level level);
    Level getDefaultLevel() const;

private:
    struct LogEntry {
        std::string message;
        Level level;
        std::chrono::system_clock::time_point timestamp;
    };

    void workerThread();
    void writeLogEntry(const LogEntry& entry);
    std::string formatTimestamp(const std::chrono::system_clock::time_point& tp) const;
    std::string levelToString(Level level) const;

    std::string filename_;
    Level defaultLevel_;
    std::queue<LogEntry> queue_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::thread worker_;
    bool stopFlag_;
};

} // namespace logger

#endif // LOGGER_H