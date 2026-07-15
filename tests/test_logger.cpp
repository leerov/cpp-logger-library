#include "../src/logger/logger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <cassert>
#include <algorithm>
#include "../src/app/console_handler.h"

using namespace logger;

#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    std::cout << "Running test: " #name " ... "; \
    test_##name(); \
    std::cout << "PASSED" << std::endl; \
} while(0)

void assertFileContains(const std::string& filename, const std::string& substring)
{
    std::ifstream file(filename);
    assert(file.is_open());
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    assert(content.find(substring) != std::string::npos);
}

void assertFileNotContains(const std::string& filename, const std::string& substring)
{
    std::ifstream file(filename);
    assert(file.is_open());
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    assert(content.find(substring) == std::string::npos);
}

void assertFileHasCount(const std::string& filename, const std::string& substring, int expectedCount)
{
    std::ifstream file(filename);
    assert(file.is_open());
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    size_t pos = 0;
    int count = 0;
    while ((pos = content.find(substring, pos)) != std::string::npos) {
        ++count;
        pos += substring.length();
    }
    assert(count == expectedCount);
}

// Test 1: Level enum values
TEST(level_enum_values)
{
    assert(static_cast<int>(Level::INFO) == 0);
    assert(static_cast<int>(Level::WARNING) == 1);
    assert(static_cast<int>(Level::ERROR) == 2);
}

// Test 2: Logger creation
TEST(logger_creation)
{
    Logger logger("test_creation.log", Level::INFO);
    assert(logger.getDefaultLevel() == Level::INFO);
}

// Test 3: Basic logging
TEST(basic_logging)
{
    const std::string filename = "test_basic.log";
    Logger logger(filename, Level::INFO);

    logger.log("Test message", Level::INFO);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    assertFileContains(filename, "Test message");
    assertFileContains(filename, "[INFO]");
}

// Test 4: Message filtering by level
TEST(level_filtering)
{
    const std::string filename = "test_filtering.log";
    Logger logger(filename, Level::WARNING);

    logger.log("Info message should be filtered", Level::INFO);
    logger.log("Warning message should appear", Level::WARNING);
    logger.log("Error message should appear", Level::ERROR);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    assertFileNotContains(filename, "Info message should be filtered");
    assertFileContains(filename, "Warning message should appear");
    assertFileContains(filename, "Error message should appear");
}

// Test 5: Change default level
TEST(change_default_level)
{
    const std::string filename = "test_change_level.log";
    Logger logger(filename, Level::INFO);

    logger.log("First message", Level::INFO);
    logger.setDefaultLevel(Level::ERROR);

    logger.log("Second message should be filtered", Level::INFO);
    logger.log("Error message should appear", Level::ERROR);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    assertFileContains(filename, "First message");
    assertFileNotContains(filename, "Second message should be filtered");
    assertFileContains(filename, "Error message should appear");
}

// Test 6: Multiple log messages
TEST(multiple_messages)
{
    const std::string filename = "test_multiple.log";
    Logger logger(filename, Level::INFO);

    for (int i = 0; i < 10; ++i) {
        logger.log("Message " + std::to_string(i), Level::INFO);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    for (int i = 0; i < 10; ++i) {
        assertFileContains(filename, "Message " + std::to_string(i));
    }
}

// Test 7: Timestamp format
TEST(timestamp_format)
{
    const std::string filename = "test_timestamp.log";
    Logger logger(filename, Level::INFO);

    logger.log("Timestamp test", Level::INFO);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::ifstream file(filename);
    assert(file.is_open());
    std::string line;
    std::getline(file, line);
    assert(line.size() >= 29);
    assert(line[0] == '[');
    assert(line[1] >= '0' && line[1] <= '9');
    assert(line[2] >= '0' && line[2] <= '9');
    assert(line[3] >= '0' && line[3] <= '9');
    assert(line[4] >= '0' && line[4] <= '9');
    assert(line[5] == '-');
    assert(line[6] >= '0' && line[6] <= '9');
    assert(line[7] >= '0' && line[7] <= '9');
    assert(line[8] == '-');
    assert(line[9] >= '0' && line[9] <= '9');
    assert(line[10] >= '0' && line[10] <= '9');
    assert(line[11] == ' ');
    assert(line[12] >= '0' && line[12] <= '9');
    assert(line[13] >= '0' && line[13] <= '9');
    assert(line[14] == ':');
    assert(line[15] >= '0' && line[15] <= '9');
    assert(line[16] >= '0' && line[16] <= '9');
    assert(line[17] == ':');
    assert(line[18] >= '0' && line[18] <= '9');
    assert(line[19] >= '0' && line[19] <= '9');
    assert(line[20] == '.');
    assert(line[21] >= '0' && line[21] <= '9');
    assert(line[22] >= '0' && line[22] <= '9');
    assert(line[23] >= '0' && line[23] <= '9');
    assert(line[24] == ']');
}

// Test 8: Level conversion to string
TEST(level_to_string)
{
    const std::string filename = "test_level_string.log";
    Logger logger(filename, Level::INFO);

    logger.log("Info test", Level::INFO);
    logger.log("Warning test", Level::WARNING);
    logger.log("Error test", Level::ERROR);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    assertFileContains(filename, "[INFO]");
    assertFileContains(filename, "[WARNING]");
    assertFileContains(filename, "[ERROR]");
}

// Test 9: Error handling - invalid file
TEST(error_handling_invalid_file)
{
    Logger logger("/invalid/path/logfile.log", Level::INFO);
    logger.log("This should not crash", Level::INFO);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // Should not crash, error message goes to cerr
}

// Test 10: Concurrent logging from multiple threads
TEST(concurrent_logging)
{
    const std::string filename = "test_concurrent.log";
    Logger logger(filename, Level::INFO);

    std::vector<std::thread> threads;
    for (int t = 0; t < 5; ++t) {
        threads.emplace_back([&logger, t]() {
            for (int i = 0; i < 20; ++i) {
                logger.log("Thread " + std::to_string(t) + " Message " + std::to_string(i), Level::INFO);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Wait for all messages to be written with retries
    int maxRetries = 50;
    int retries = 0;
    bool allMessagesWritten = false;

    while (retries < maxRetries && !allMessagesWritten) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        std::ifstream file(filename);
        assert(file.is_open());
        std::string content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        size_t pos = 0;
        int count = 0;
        while ((pos = content.find("[INFO]", pos)) != std::string::npos) {
            ++count;
            pos += 6; // length of "[INFO]"
        }

        if (count == 100) {
            allMessagesWritten = true;
        }
        ++retries;
    }

    assert(allMessagesWritten && "Failed to write all 100 messages within timeout");
}

// Test 11: Change level while logging
TEST(change_level_during_logging)
{
    const std::string filename = "test_dynamic_level.log";
    Logger logger(filename, Level::INFO);

    logger.log("Message 1", Level::INFO);
    logger.setDefaultLevel(Level::ERROR);
    logger.log("Message 2 filtered", Level::INFO);
    logger.log("Message 3 error", Level::ERROR);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    assertFileContains(filename, "Message 1");
    assertFileNotContains(filename, "Message 2 filtered");
    assertFileContains(filename, "Message 3 error");
}

// Test 12: Console parser - parseLevel function
TEST(console_parse_level)
{
    assert(console::parseLevel("INFO") == logger::Level::INFO);
    assert(console::parseLevel("WARNING") == logger::Level::WARNING);
    assert(console::parseLevel("ERROR") == logger::Level::ERROR);
    assert(console::parseLevel("info") == logger::Level::INFO);
    assert(console::parseLevel("Warning") == logger::Level::WARNING);
    assert(console::parseLevel("error") == logger::Level::ERROR);
    assert(console::parseLevel("INVALID") == logger::Level::INFO);
    assert(console::parseLevel("") == logger::Level::INFO);
}

// Test 13: Console parser - isValidLevel function
TEST(console_is_valid_level)
{
    assert(console::isValidLevel("INFO") == true);
    assert(console::isValidLevel("WARNING") == true);
    assert(console::isValidLevel("ERROR") == true);
    assert(console::isValidLevel("info") == true);
    assert(console::isValidLevel("Warning") == true);
    assert(console::isValidLevel("error") == true);
    assert(console::isValidLevel("INVALID") == false);
    assert(console::isValidLevel("") == false);
    assert(console::isValidLevel("INFO ") == true);
}

// Test 14: Console parser - parseInput function
TEST(console_parse_input)
{
    logger::Level defaultLevel = logger::Level::INFO;

    auto result1 = console::parseInput("Test message", defaultLevel);
    assert(result1.first == "Test message");
    assert(result1.second == logger::Level::INFO);

    auto result2 = console::parseInput("Error message ERROR", defaultLevel);
    assert(result2.first == "Error message");
    assert(result2.second == logger::Level::ERROR);

    auto result3 = console::parseInput("Warning WARNING", defaultLevel);
    assert(result3.first == "Warning");
    assert(result3.second == logger::Level::WARNING);

    auto result4 = console::parseInput("Multiple words in message INFO", defaultLevel);
    assert(result4.first == "Multiple words in message");
    assert(result4.second == logger::Level::INFO);

    auto result5 = console::parseInput("", defaultLevel);
    assert(result5.first == "");
    assert(result5.second == logger::Level::INFO);

    auto result6 = console::parseInput("exit", defaultLevel);
    assert(result6.first == "exit");
    assert(result6.second == logger::Level::INFO);
}

// Test 15: Logger - file permission handling
TEST(file_permission_handling)
{
    // Создаем файл только для чтения
    const std::string filename = "test_readonly.log";
    std::ofstream testFile(filename);
    testFile << "test" << std::endl;
    testFile.close();

    // Меняем права на только чтение (только в Unix-подобных системах)
    // В Windows этот тест будет пропущен
    #ifdef __unix__
    chmod(filename.c_str(), 0444);

    Logger logger(filename, logger::Level::INFO);
    // Должен вывести предупреждение, но не упасть
    logger.log("Test message", logger::Level::INFO);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Восстанавливаем права для удаления
    chmod(filename.c_str(), 0644);
    #endif
}
int main()
{
    std::cout << "Starting logger library tests..." << std::endl;
    std::cout << "================================" << std::endl;

    RUN_TEST(level_enum_values);
    RUN_TEST(logger_creation);
    RUN_TEST(basic_logging);
    RUN_TEST(level_filtering);
    RUN_TEST(change_default_level);
    RUN_TEST(multiple_messages);
    RUN_TEST(timestamp_format);
    RUN_TEST(level_to_string);
    RUN_TEST(error_handling_invalid_file);
    RUN_TEST(concurrent_logging);
    RUN_TEST(change_level_during_logging);
    RUN_TEST(console_parse_level);
    RUN_TEST(console_is_valid_level);
    RUN_TEST(console_parse_input);
    RUN_TEST(file_permission_handling);

    std::cout << "================================" << std::endl;
    std::cout << "All tests PASSED!" << std::endl;

    return 0;
}