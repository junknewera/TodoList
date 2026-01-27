#include "json.hpp"
#include <algorithm>
#include <charconv>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

using json = nlohmann::json;

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
enum class CustomError { Ok, InvalidNumber, NoSuchTask, ParseError, IoError };
struct ResultIndex {
  CustomError code;
  size_t index = 0;
};
struct ResolvedId {
  CustomError code = CustomError::InvalidNumber;
  std::optional<uint64_t> id;
};

ResolvedId resolveIdFromUserNumber(const AppState &state,
                                   const std::string &flag);
std::string trim(const std::string &userInput);
void add(AppState &state, const std::string &userInput);
void list(const AppState &state);
CustomError edit(AppState &state, uint64_t id, EditType type);
ResultIndex parseIndex(const size_t listSize, const std::string &userInput);
std::optional<size_t> findIndexById(const AppState &state, uint64_t id);
void printError(const CustomError &err);
CustomError saveState(const AppState &state, const std::string &path);
CustomError loadState(AppState &state, const std::string &path);
void applyEditAndSave(AppState &state, const std::string &flag, EditType type,
                      const std::string &path);
[[noreturn]] inline void unreachable() { std::abort(); };

int main() {
  std::string userInput;
  std::string cmd;
  std::string flag;
  AppState state;
  std::string path = "todo.json";

  printError(loadState(state, path));

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
      printError(saveState(state, path));
    } else if (cmd == "list") {
      list(state);
    } else if (cmd == "done") {
      applyEditAndSave(state, flag, EditType::done, path);
    } else if (cmd == "del") {
      applyEditAndSave(state, flag, EditType::del, path);
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

CustomError edit(AppState &state, uint64_t id, EditType type) {
  auto index = findIndexById(state, id);
  if (!index) {
    return CustomError::NoSuchTask;
  }
  switch (type) {
  case EditType::done:
    state.tasks[*index].done = true;
    break;
  case EditType::del:
    state.tasks.erase(state.tasks.begin() + *index);
    break;
  }
  return CustomError::Ok;
}
ResultIndex parseIndex(const size_t listSize, const std::string &userInput) {
  ResultIndex index;
  if (userInput.empty()) {
    index.code = CustomError::InvalidNumber;
    return index;
  }
  int num;

  auto [ptr, err] = std::from_chars(userInput.data(),
                                    userInput.data() + userInput.size(), num);
  if (!(err == std::errc()) || !(userInput.data() + userInput.size() == ptr)) {
    index.code = CustomError::InvalidNumber;
    return index;
  }
  if (num < 1 || static_cast<size_t>(num) > listSize) {
    index.code = CustomError::NoSuchTask;
    return index;
  }
  index.code = CustomError::Ok;
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
  std::string input = flag;
  if (flag.empty()) {
    std::cout << "Enter task number: ";
    std::getline(std::cin, input);
  }
  input = trim(input);

  ResultIndex index = parseIndex(state.tasks.size(), input);
  if (index.code != CustomError::Ok) {
    return {index.code, std::nullopt};
  }
  return {CustomError::Ok, state.tasks[index.index].id};
}
std::optional<size_t> findIndexById(const AppState &state, uint64_t id) {
  for (size_t i = 0; i < state.tasks.size(); i++) {
    if (state.tasks[i].id == id) {
      return i;
    }
  }
  return {};
}
void printError(const CustomError &err) {
  switch (err) {
  case CustomError::Ok:
    return;
  case CustomError::InvalidNumber:
    std::cout << "Error: Invalid number\n";
    return;
  case CustomError::NoSuchTask:
    std::cout << "Error: No such task\n";
    return;
  case CustomError::ParseError:
    std::cout << "Error: Parse Error\n";
    return;
  case CustomError::IoError:
    std::cout << "Error: Input/Output Error\n";
    return;
  }
}
CustomError saveState(const AppState &state, const std::string &path) {
  json root;
  root["next_id"] = state.next_id;
  root["tasks"] = json::array();
  for (auto &task : state.tasks) {
    json t;
    t["id"] = task.id;
    t["text"] = task.text;
    t["done"] = task.done;
    root["tasks"].push_back(t);
  }
  std::ofstream file(path);
  if (!file) {
    return CustomError::IoError;
  }
  file << root.dump();
  if (!file) {
    return CustomError::IoError;
  }
  return CustomError::Ok;
}
CustomError loadState(AppState &state, const std::string &path) {
  std::ifstream file(path);
  json data;
  state.tasks.clear();
  uint64_t max = 0;

  if (!file) {
    return CustomError::Ok;
  }
  try {
    file >> data;
    data.at("next_id").get_to(state.next_id);
    for (const auto &task : data.at("tasks")) {
      Task t;
      task.at("id").get_to(t.id);
      max = std::max(max, t.id);
      task.at("text").get_to(t.text);
      task.at("done").get_to(t.done);
      state.tasks.push_back(t);
    }
  } catch (const std::exception &e) {
    return CustomError::ParseError;
  }
  state.next_id = std::max(state.next_id, max + 1);
  return CustomError::Ok;
}

void applyEditAndSave(AppState &state, const std::string &flag, EditType type,
                      const std::string &path) {
  auto rid = resolveIdFromUserNumber(state, flag);
  if (rid.code != CustomError::Ok) {
    printError(rid.code);
    return;
  }
  auto r = edit(state, rid.id.value(), type);
  printError(r);
  if (r == CustomError::Ok) {
    printError(saveState(state, path));
  }
}
