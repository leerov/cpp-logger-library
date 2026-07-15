#include "../logger/logger.h"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <csignal>

volatile std::sig_atomic_t gRunning = 1;

void signalHandler(int signal)
{
    (void)signal;
    gRunning = 0;
}

logger::Level parseLevel(const std::string& str)
{
    std::string upper = str;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    if (upper == "INFO") return logger::Level::INFO;
    if (upper == "WARNING") return logger::Level::WARNING;
    if (upper == "ERROR") return logger::Level::ERROR;

    return logger::Level::INFO;
}

bool isValidLevel(const std::string& str)
{
    std::string upper = str;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return upper == "INFO" || upper == "WARNING" || upper == "ERROR";
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <log_file> <default_level>" << std::endl;
        std::cerr << "Levels: INFO, WARNING, ERROR" << std::endl;
        return 1;
    }

    std::string logFile = argv[1];
    std::string levelStr = argv[2];

    if (!isValidLevel(levelStr)) {
        std::cerr << "Error: Invalid level. Use INFO, WARNING, or ERROR" << std::endl;
        return 1;
    }

    logger::Level defaultLevel = parseLevel(levelStr);

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    logger::Logger logger(logFile, defaultLevel);

    std::cout << "Logger initialized. Log file: " << logFile
              << ", default level: " << levelStr << std::endl;
    std::cout << "Enter messages (format: <message> [level])" << std::endl;
    std::cout << "Commands: change_level <level>, exit" << std::endl;
    std::cout << "> " << std::flush;

    std::string line;
    while (gRunning && std::getline(std::cin, line)) {
        if (line.empty()) {
            std::cout << "> " << std::flush;
            continue;
        }

        std::istringstream iss(line);
        std::string firstWord;
        iss >> firstWord;

        if (firstWord == "exit" || firstWord == "quit") {
            break;
        }

        if (firstWord == "change_level") {
            std::string newLevel;
            if (iss >> newLevel) {
                if (isValidLevel(newLevel)) {
                    logger.setDefaultLevel(parseLevel(newLevel));
                    std::cout << "Default level changed to: " << newLevel << std::endl;
                } else {
                    std::cerr << "Invalid level. Use INFO, WARNING, or ERROR" << std::endl;
                }
            } else {
                std::cerr << "Usage: change_level <INFO|WARNING|ERROR>" << std::endl;
            }
            std::cout << "> " << std::flush;
            continue;
        }

        std::string message = line;
        logger::Level level = defaultLevel;

        std::string lastWord;
        if (iss >> lastWord) {
            if (isValidLevel(lastWord)) {
                size_t pos = line.rfind(lastWord);
                if (pos != std::string::npos) {
                    message = line.substr(0, pos);
                    while (!message.empty() && message.back() == ' ') {
                        message.pop_back();
                    }
                    level = parseLevel(lastWord);
                }
            }
        }

        logger.log(message, level);
        std::cout << "> " << std::flush;
    }

    std::cout << "Shutting down..." << std::endl;
    return 0;
}