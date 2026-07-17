#include "console_handler.h"
#include "../logger/logger.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>
#include <utility>

namespace console {

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
    std::string trimmed = str;
    size_t start = trimmed.find_first_not_of(" \t\n\r");
    size_t end = trimmed.find_last_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return false;
    }
    trimmed = trimmed.substr(start, end - start + 1);
    std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(), ::toupper);
    return trimmed == "INFO" || trimmed == "WARNING" || trimmed == "ERROR";
}

std::pair<std::string, logger::Level> parseInput(const std::string& line, logger::Level defaultLevel)
{
    if (line.empty()) {
        return {"", defaultLevel};
    }

    std::istringstream iss(line);
    std::string firstWord;
    iss >> firstWord;

    // Проверяем, не является ли первое слово командой
    if (firstWord == "exit" || firstWord == "quit" ||
        firstWord == "change_level") {
        return {line, defaultLevel};
    }

    std::string message = line;
    logger::Level level = defaultLevel;

    std::string lastWord;
    std::string temp;
    std::vector<std::string> words;

    // Разбиваем строку на слова
    std::istringstream wordStream(line);
    while (wordStream >> temp) {
        words.push_back(temp);
    }

    // Если есть последнее слово и это валидный уровень - извлекаем его
    if (!words.empty()) {
        std::string last = words.back();
        if (isValidLevel(last)) {
            // Удаляем последнее слово из сообщения
            size_t pos = line.rfind(last);
            if (pos != std::string::npos) {
                message = line.substr(0, pos);
                while (!message.empty() && message.back() == ' ') {
                    message.pop_back();
                }
                level = parseLevel(last);
            }
        }
    }

    return {message, level};
}

} // namespace console