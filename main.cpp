#include <cctype>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
struct Task {
  std::string text;
  bool done = false;
};

using TaskAction = bool (*)(std::vector<Task> &, int);

enum class CmdResult { Ok, InvalidNumber, NoSuchTask };

std::optional<int> parseInt(const std::string taskNum);
void add(std::vector<Task> &todoList, const std::string userInput);
void list(const std::vector<Task> &todoList);
bool done(std::vector<Task> &todoList, const int userInput);
bool delTask(std::vector<Task> &todoList, const int userInput);
CmdResult getCmdResult(std::vector<Task> &todoList, const std::string flag,
                       TaskAction action);

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
      std::string input;
      CmdResult cmdRes;
      if (flag.empty()) {
        std::cout << "Enter task number: ";
        std::getline(std::cin, input);
      } else {
        input = flag;
      }
      cmdRes = getCmdResult(todoList, input, done);
      switch (cmdRes) {
      case CmdResult::Ok:
        break;
      case CmdResult::InvalidNumber:
        std::cout << "Error: not a number\n";
        break;
      case CmdResult::NoSuchTask:
        std::cout << "Error: no such task\n";
        break;
      }
    } else if (cmd == "del") {
      std::string input;
      CmdResult cmdRes;
      if (flag.empty()) {
        std::cout << "Enter task number: ";
        std::getline(std::cin, input);
      } else {
        input = flag;
      }
      cmdRes = getCmdResult(todoList, input, delTask);
      switch (cmdRes) {
      case CmdResult::Ok:
        break;
      case CmdResult::InvalidNumber:
        std::cout << "Error: not a number\n";
        break;
      case CmdResult::NoSuchTask:
        std::cout << "Error: no such task\n";
        break;
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
bool done(std::vector<Task> &todoList, const int userInput) {
  if (userInput < 1 || static_cast<size_t>(userInput) > todoList.size()) {
    return false;
  }
  todoList[userInput - 1].done = true;
  return true;
}
bool delTask(std::vector<Task> &todoList, const int userInput) {
  if (userInput < 1 || static_cast<size_t>(userInput) > todoList.size()) {
    return false;
  }
  todoList.erase(todoList.begin() + userInput - 1);
  return true;
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

CmdResult getCmdResult(std::vector<Task> &todoList, const std::string flag,
                       TaskAction action) {
  std::optional<int> num = parseInt(flag);
  if (!num) {
    return CmdResult::InvalidNumber;
  } else {
    bool isCompleted = action(todoList, *num);
    if (!isCompleted) {
      return CmdResult::NoSuchTask;
    }
  }
  return CmdResult::Ok;
}
