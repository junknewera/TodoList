#include <iostream>
#include <limits>
#include <string>
#include <variant>
#include <vector>

struct Task {
  std::string text;
  bool done = false;
};

int taskNumRequest();
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
      int intFlag;
      if (!flag.empty()) {
        intFlag = std::stoi(flag);
      } else {
        intFlag = 0;
      }
      done(todoList, intFlag);
    } else if (cmd == "del") {
      int intFlag;
      if (!flag.empty()) {
        intFlag = std::stoi(flag);
      } else {
        intFlag = 0;
      }
      delTask(todoList, intFlag);
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
    taskNum = taskNumRequest();
  } else {
    taskNum = userInput;
  }

  if (taskNum < 1 || static_cast<size_t>(taskNum) > todoList.size()) {
    std::cout << "No such task";
    return;
  }
  todoList[userInput - 1].done = true;

  std::cout << std::endl;
}
void delTask(std::vector<Task> &todoList, const int userInput) {
  int taskNum;
  if (!userInput) {
    taskNum = taskNumRequest();
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
int taskNumRequest() {
  int taskNum;
  std::cout << "Enter task number: ";
  std::cin >> taskNum;
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  return taskNum;
}
