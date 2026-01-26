#include <charconv>
#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

struct Task {
  uint64_t id;
  std::string text;
  bool done = false;
};
struct AppState {
  std::vector<Task> tasks;
  uint64_t next_id = 1;
};
enum class EditType { del, done };
enum class CmdResult { Ok, InvalidNumber, NoSuchTask };
struct ResultIndex {
  CmdResult code;
  size_t index = 0;
};
struct ResolvedId {
  CmdResult code = CmdResult::InvalidNumber;
  std::optional<uint64_t> id;
};

ResolvedId resolveIdFromUserNumber(const AppState &state,
                                   const std::string &flag);
std::string trim(const std::string &userInput);
void add(AppState &state, const std::string &userInput);
void list(const AppState &state);
CmdResult edit(AppState &state, uint64_t id, EditType type);
ResultIndex parseIndex(const size_t listSize, const std::string &userInput);
std::optional<size_t> findIndexById(const AppState &state, uint64_t id);
void printError(const CmdResult &err);

int main() {
  std::string userInput;
  std::string cmd;
  std::string flag;
  AppState state;

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
      add(state, flag);
    } else if (cmd == "list") {
      list(state);
    } else if (cmd == "done") {
      auto id = resolveIdFromUserNumber(state, flag);
      if (id.code != CmdResult::Ok) {
        printError(id.code);
      } else {
        printError(edit(state, *id.id, EditType::done));
      }
    } else if (cmd == "del") {
      auto id = resolveIdFromUserNumber(state, flag);
      if (id.code != CmdResult::Ok) {
        printError(id.code);
      } else {
        printError(edit(state, *id.id, EditType::done));
      }

    } else {
      std::cout << "Unknown command: " << cmd << std::endl;
    }
    std::cout << std::endl;
  }
}

void add(AppState &state, const std::string &userInput) {
  Task task;
  task.id = state.next_id;
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

  state.tasks.push_back(task);
  state.next_id++;
}
void list(const AppState &state) {
  if (state.tasks.empty()) {
    std::cout << "Todo List is empty!\n";
  } else {
    int i = 1;
    for (const auto &task : state.tasks) {
      std::cout << i++ << " [id=" << task.id << "]"
                << (task.done ? " 🗹 " : " ☐ ");
      std::cout << task.text << std::endl;
    }
  }
}

CmdResult edit(AppState &state, uint64_t id, EditType type) {
  auto index = findIndexById(state, id);
  if (!index) {
    return CmdResult::NoSuchTask;
  }
  switch (type) {
  case EditType::done:
    state.tasks[*index].done = true;
    break;
  case EditType::del:
    state.tasks.erase(state.tasks.begin() + *index);
    break;
  }
  return CmdResult::Ok;
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
ResolvedId resolveIdFromUserNumber(const AppState &state,
                                   const std::string &flag) {
  ResolvedId resId;
  std::string input;
  if (flag.empty()) {
    std::cout << "Enter task number: ";
    std::getline(std::cin, input);
  } else {
    input = flag;
  }
  input = trim(input);
  ResultIndex index = parseIndex(state.tasks.size(), input);
  switch (index.code) {
  case CmdResult::Ok:
    resId.code = CmdResult::Ok;
    resId.id = state.tasks[index.index].id;
    return resId;
  case CmdResult::InvalidNumber:
    resId.code = CmdResult::InvalidNumber;
    break;
  case CmdResult::NoSuchTask:
    resId.code = CmdResult::NoSuchTask;
    break;
  }
  return resId;
}
std::optional<size_t> findIndexById(const AppState &state, uint64_t id) {
  for (size_t i = 0; i < state.tasks.size(); i++) {
    if (state.tasks[i].id == id) {
      return i;
    }
  }
  return {};
}

void printError(const CmdResult &err) {

  switch (err) {
  case CmdResult::Ok:
    break;
  case CmdResult::InvalidNumber:
    std::cout << "Error: Invalid number\n";
    break;
  case CmdResult::NoSuchTask:
    std::cout << "Error: No such task\n";
    break;
  }
}
