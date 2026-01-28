CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra
TARGET = main
TARGET_DEL = main
SRCS = src/Task.cpp src/TaskManager.cpp src/main.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
