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
APP_OBJ = $(OBJ_DIR)/main.o $(OBJ_DIR)/console_handler.o
TEST_OBJ = $(OBJ_DIR)/test_logger.o

.PHONY: all clean lib app test
.PHONY: all clean lib app test help

all: lib app test
help:
	@echo "Available targets:"
	@echo "  make all        - Build library, app, and tests (default)"
	@echo "  make lib        - Build dynamic library only (liblogger.so)"
	@echo "  make app        - Build console application only (logger_app)"
	@echo "  make test       - Build and run tests"
	@echo "  make clean      - Remove all build artifacts"
	@echo ""
	@echo "Usage examples:"
	@echo "  make all        # Build everything"
	@echo "  make test       # Run tests"
	@echo "  make clean      # Clean up"

lib: $(TARGET_LIB)

app: $(TARGET_APP)
test: $(TARGET_TEST)
	@echo "Running tests..."
	@LD_LIBRARY_PATH=build $(TARGET_TEST)
test: $(TARGET_TEST)
	@echo "Running tests..."
	@LD_LIBRARY_PATH=build $(TARGET_TEST)

test-clean: clean
	@rm -rf test_*.log
	@echo "All test logs cleaned"

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

test-clean:
	rm -rf test_*.log
	@echo "All test logs cleaned"
