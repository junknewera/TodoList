#include <cctype>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

struct Task {
  std::string text;
  bool done = false;
};

std::optional<int> parseInt(const std::string taskNum);
std::optional<int> taskNumRequest();
void add(std::vector<Task> &todoList, const std::string userInput);
void list(const std::vector<Task> &todoList);
void done(std::vector<Task> &todoList, const int userInput);
void delTask(std::vector<Task> &todoList, const int userInput);

int main() {
  std::vector<Task> todoList{};
  std::string userInput;
  std::string cmd;
  std::string flag;

  std::cout << "Todo List." << std::endl;
  while (true) {
    std::cout << "Enter one of the following:\n"
                 "add [task] - to add any task\n"
                 "list - to check current tasks\n"
                 "done [num] - to mark any task as done\n"
                 "del [num] - to remove any task\n"
                 "q - to quit\n";

    std::getline(std::cin, userInput);
    auto split = userInput.find(' ');
    if (split != std::string::npos) {
      cmd = userInput.substr(0, split);
      flag = userInput.substr(split + 1, userInput.size());
    } else {
      cmd = userInput;
      flag.clear();
    }

    if (cmd == "q") {
      break;
    } else if (cmd == "add") {
      add(todoList, flag);
    } else if (cmd == "list") {
      list(todoList);
    } else if (cmd == "done") {
      std::optional<int> optInt = parseInt(flag);
      if (!optInt) {
        if (flag.empty()) {
          done(todoList, 0);
        } else {
          std::cout << "Usage: done [num]\n";
          continue;
        }
      } else {
        done(todoList, *optInt);
      }
    } else if (cmd == "del") {
      std::optional<int> optInt = parseInt(flag);
      if (!optInt) {
        if (flag.empty()) {
          delTask(todoList, 0);
        } else {
          std::cout << "usage: del [num]\n";
          continue;
        }
      } else {
        delTask(todoList, *optInt);
      }
    } else {
      std::cout << "Unknown command: " << cmd << std::endl;
    }
    std::cout << std::endl;
  }
}

void add(std::vector<Task> &todoList, const std::string userInput) {
  Task task;
  if (userInput.empty()) {
    std::cout << "Enter task name: ";
    std::getline(std::cin, task.text);
    if (task.text.empty()) {
      std::cout << "No task provided.\n";
      return;
    }
    std::cout << std::endl;
  } else {
    task.text = userInput;
  }

  todoList.push_back(task);
}
void list(const std::vector<Task> &todoList) {
  if (todoList.empty()) {
    std::cout << "Todo List is empty!\n";
  } else {
    int i = 1;
    for (const auto &task : todoList) {
      std::cout << i++ << (task.done ? " 🗹 " : " ☐ ");
      std::cout << task.text << std::endl;
    }
  }
}
void done(std::vector<Task> &todoList, const int userInput) {
  int taskNum;
  if (!userInput) {
    auto task = taskNumRequest();
    if (!task) {
      std::cout << "Error: not a number\n";
      return;
    } else {
      taskNum = *task;
    }
  } else {
    taskNum = userInput;
  }

  if (taskNum < 1 || static_cast<size_t>(taskNum) > todoList.size()) {
    std::cout << "No such task";
    return;
  }
  todoList[taskNum - 1].done = true;

  std::cout << std::endl;
}
void delTask(std::vector<Task> &todoList, const int userInput) {
  int taskNum;
  if (!userInput) {
    auto task = taskNumRequest();
    if (!task) {
      std::cout << "Error: not a number\n";
      return;
    } else {
      taskNum = *task;
    }
  } else {
    taskNum = userInput;
  }
  if (taskNum < 1 || static_cast<size_t>(taskNum) > todoList.size()) {
    std::cout << "No such task";
    return;
  }
  todoList.erase(todoList.begin() + taskNum - 1);

  std::cout << std::endl;
}
std::optional<int> taskNumRequest() {
  std::string taskNum;

  std::cout << "Enter task number: ";
  std::getline(std::cin, taskNum);

  return parseInt(taskNum);
}
std::optional<int> parseInt(const std::string taskNum) {

  if (taskNum.empty()) {
    return {};
  }
  for (const char &ch : taskNum) {
    auto uc = static_cast<unsigned char>(ch);
    if (!std::isdigit(uc)) {
      return {};
    }
  }
  int numToInt;
  numToInt = std::stoi(taskNum);
  return numToInt;
}
