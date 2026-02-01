CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra
TARGET = main
TARGET_DEL = main
TEST_SRCS = tests/test_task.cpp
TEST_TARGET = test/test_task
SRCS = src/Task.cpp src/TaskManager.cpp src/main.cpp src/Command.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TEST TARGET)
	./$(TEST TARGET)

$(TEST_TARGET): $(TEST_SRCS) src/Task.cpp
	$(CXX) $(CXXFLAGS) $(TEST_SRCS) src/Task.cpp -o $(TEST_TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
