#include <charconv>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

struct Task {
  std::string text;
  bool done = false;
};
enum class EditType { del, done };

enum class CmdResult { Ok, InvalidNumber, NoSuchTask };
struct ResultIndex {
  CmdResult code;
  size_t index = 0;
};

std::string trim(const std::string &userInput);
void add(std::vector<Task> &todoList, const std::string &userInput);
void list(const std::vector<Task> &todoList);
void edit(std::vector<Task> &todoList, const size_t index, const EditType type);
ResultIndex parseIndex(const size_t listSize, const std::string &userInput);
std::optional<size_t> runIndexCommand(const size_t size,
                                      const std::string &flag);

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
      cmd = trim(userInput.substr(0, split));
      flag = trim(userInput.substr(split + 1, userInput.size()));
    } else {
      cmd = trim(userInput);
      flag.clear();
    }

    if (cmd == "q") {
      break;
    } else if (cmd == "add") {
      add(todoList, flag);
    } else if (cmd == "list") {
      list(todoList);
    } else if (cmd == "done") {
      auto index = runIndexCommand(todoList.size(), flag);
      if (index) {
        edit(todoList, *index, EditType::done);
      }
    } else if (cmd == "del") {
      auto index = runIndexCommand(todoList.size(), flag);
      if (index) {
        edit(todoList, *index, EditType::del);
      }
    } else {
      std::cout << "Unknown command: " << cmd << std::endl;
    }
    std::cout << std::endl;
  }
}

void add(std::vector<Task> &todoList, const std::string &userInput) {
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

void edit(std::vector<Task> &todoList, const size_t index,
          const EditType type) {

  switch (type) {
  case EditType::done:
    todoList[index].done = true;
    break;
  case EditType::del:
    todoList.erase(todoList.begin() + index);
    break;
  }
}
std::optional<size_t> runIndexCommand(const size_t size,
                                      const std::string &flag) {
  std::string input;
  if (flag.empty()) {
    std::cout << "Enter task number: ";
    std::getline(std::cin, input);
  } else {
    input = flag;
  }
  ResultIndex index = parseIndex(size, input);
  switch (index.code) {
  case CmdResult::Ok:
    return index.index;
  case CmdResult::InvalidNumber:
    std::cout << "Error: not a number\n";
    break;
  case CmdResult::NoSuchTask:
    std::cout << "Error: no such task\n";
    break;
  }
  return {};
}
ResultIndex parseIndex(const size_t listSize, const std::string &userInput) {
  ResultIndex index;
  if (userInput.empty()) {
    index.code = CmdResult::InvalidNumber;
    return index;
  }
  int num;

  auto [ptr, err] = std::from_chars(userInput.data(),
                                    userInput.data() + userInput.size(), num);
  if (!(err == std::errc()) || !(userInput.data() + userInput.size() == ptr)) {
    index.code = CmdResult::InvalidNumber;
    return index;
  }
  if (num < 1 || static_cast<size_t>(num) > listSize) {
    index.code = CmdResult::NoSuchTask;
    return index;
  }
  index.code = CmdResult::Ok;
  index.index = static_cast<size_t>(num - 1);
  return index;
}
std::string trim(const std::string &userInput) {
  const auto first = userInput.find_first_not_of(" \t");
  if (first == std::string::npos) {
    return "";
  }
  const auto last = userInput.find_last_not_of(" \t");
  return userInput.substr(first, last - first + 1);
}
