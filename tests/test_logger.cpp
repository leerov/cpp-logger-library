#include "../src/logger/logger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <cassert>
#include <algorithm>

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
    assert(line.size() >= 24);
    assert(line[0] == '[');
    assert(line[1] >= '0' && line[1] <= '9');
    assert(line[2] >= '0' && line[2] <= '9');
    assert(line[3] >= '0' && line[3] <= '9');
    assert(line[4] == '-');
    assert(line[5] >= '0' && line[5] <= '9');
    assert(line[6] >= '0' && line[6] <= '9');
    assert(line[7] == '-');
    assert(line[8] >= '0' && line[8] <= '9');
    assert(line[9] >= '0' && line[9] <= '9');
    assert(line[10] == ' ');
    assert(line[11] >= '0' && line[11] <= '9');
    assert(line[12] >= '0' && line[12] <= '9');
    assert(line[13] == ':');
    assert(line[14] >= '0' && line[14] <= '9');
    assert(line[15] >= '0' && line[15] <= '9');
    assert(line[16] == ':');
    assert(line[17] >= '0' && line[17] <= '9');
    assert(line[18] >= '0' && line[18] <= '9');
    assert(line[19] == '.');
    assert(line[20] >= '0' && line[20] <= '9');
    assert(line[21] >= '0' && line[21] <= '9');
    assert(line[22] >= '0' && line[22] <= '9');
    assert(line[23] == ']');
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

    // Count total messages in file
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
    assert(count == 100); // 5 threads * 20 messages
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

    std::cout << "================================" << std::endl;
    std::cout << "All tests PASSED!" << std::endl;

    return 0;
}