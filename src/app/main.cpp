#include "../logger/logger.h"
#include "console_handler.h"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>

volatile std::sig_atomic_t gRunning = 1;

void signalHandler(int signal)
{
    (void)signal;
    gRunning = 0;
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

    if (!console::isValidLevel(levelStr)) {
        std::cerr << "Error: Invalid level. Use INFO, WARNING, or ERROR" << std::endl;
        return 1;
    }

    logger::Level defaultLevel = console::parseLevel(levelStr);

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    // Проверяем права доступа к файлу
    if (access(logFile.c_str(), F_OK) == 0) {
        if (access(logFile.c_str(), W_OK) != 0) {
            std::cerr << "Error: No write permission for log file: " << logFile << std::endl;
            return 1;
        }
    } else {
        // Файл не существует - проверяем директорию
        std::string dir = logFile;
        size_t pos = dir.find_last_of("/\\");
        if (pos != std::string::npos) {
            dir = dir.substr(0, pos);
            if (access(dir.c_str(), W_OK) != 0) {
                std::cerr << "Error: No write permission for directory: " << dir << std::endl;
                return 1;
            }
        }
    }

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
                if (console::isValidLevel(newLevel)) {
                    logger.setDefaultLevel(console::parseLevel(newLevel));
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

        auto [message, level] = console::parseInput(line, defaultLevel);
        // Проверяем, не является ли это командой (exit/quit)
        if (message == "exit" || message == "quit" || message.find("change_level") != std::string::npos) {
            continue;
        }

        logger.log(message, level);
        std::cout << "> " << std::flush;
    }

    std::cout << "Shutting down..." << std::endl;
    return 0;
}