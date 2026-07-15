CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -fPIC
LDFLAGS = -pthread

TARGET_LIB = build/liblogger.so
TARGET_APP = build/logger_app

SRC_DIR = src
OBJ_DIR = build/obj
LIB_SRC = $(SRC_DIR)/logger/logger.cpp
APP_SRC = $(SRC_DIR)/app/main.cpp

LIB_OBJ = $(OBJ_DIR)/logger.o
APP_OBJ = $(OBJ_DIR)/main.o

.PHONY: all clean lib app

all: lib app

lib: $(TARGET_LIB)

app: $(TARGET_APP)

$(TARGET_LIB): $(LIB_OBJ)
	@mkdir -p $(dir $@)
	$(CXX) -shared -o $@ $^ $(LDFLAGS)

$(TARGET_APP): $(APP_OBJ) $(TARGET_LIB)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $< -Lbuild -llogger $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/logger/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/app/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET_LIB) $(TARGET_APP)