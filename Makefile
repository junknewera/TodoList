CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra
BUILD_DIR = build
TARGET = main
TARGET_DEL = main
TEST_SRCS = tests/test_task.cpp tests/test_utils.cpp tests/test_task_manager.cpp tests/test_command.cpp
TEST_TARGET = tests/test_all
TEST_DEPS = src/Task.cpp src/TaskManager.cpp src/Command.cpp src/Utils.cpp
SRCS = src/Task.cpp src/TaskManager.cpp src/main.cpp src/Command.cpp src/Utils.cpp
OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRCS) $(TEST_DEPS)
	$(CXX) $(CXXFLAGS) $(TEST_SRCS) $(TEST_DEPS) -o $(TEST_TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)
