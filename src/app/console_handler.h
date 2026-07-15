#ifndef CONSOLE_HANDLER_H
#define CONSOLE_HANDLER_H

#include <string>

namespace console {

/**
 * @brief Парсит строку уровня важности в перечисление Level
 * @param str Строка с названием уровня (регистронезависимо)
 * @return Значение Level::INFO, Level::WARNING или Level::ERROR
 *         По умолчанию возвращает Level::INFO
 */
logger::Level parseLevel(const std::string& str);

/**
 * @brief Проверяет, является ли строка корректным названием уровня
 * @param str Строка для проверки
 * @return true если строка равна INFO, WARNING или ERROR (регистронезависимо)
 */
bool isValidLevel(const std::string& str);

/**
 * @brief Выделяет сообщение и уровень из строки ввода пользователя
 * @param line Исходная строка ввода
 * @param defaultLevel Уровень по умолчанию
 * @return Пара {сообщение, уровень}
 */
std::pair<std::string, logger::Level> parseInput(const std::string& line, logger::Level defaultLevel);

} // namespace console

#endif // CONSOLE_HANDLER_H