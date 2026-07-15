CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -fPIC
LDFLAGS = -pthread

TARGET_LIB = build/liblogger.so
TARGET_APP = build/logger_app
TARGET_TEST = build/test_logger

SRC_DIR = src
OBJ_DIR = build/obj
LIB_SRC = $(SRC_DIR)/logger/logger.cpp
APP_SRC = $(SRC_DIR)/app/main.cpp
TEST_SRC = tests/test_logger.cpp

LIB_OBJ = $(OBJ_DIR)/logger.o
APP_OBJ = $(OBJ_DIR)/main.o
TEST_OBJ = $(OBJ_DIR)/test_logger.o

.PHONY: all clean lib app test

all: lib app test

lib: $(TARGET_LIB)

app: $(TARGET_APP)

$(TARGET_LIB): $(LIB_OBJ)
	@mkdir -p $(dir $@)
	$(CXX) -shared -o $@ $^ $(LDFLAGS)

$(TARGET_APP): $(APP_OBJ) $(TARGET_LIB)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $< -Lbuild -llogger $(LDFLAGS)

$(TARGET_TEST): $(TEST_OBJ) $(TARGET_LIB)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $< -Lbuild -llogger $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/logger/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: tests/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/app/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET_LIB) $(TARGET_APP) $(TARGET_TEST)