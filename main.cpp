#include "json.hpp"
#include <algorithm>
#include <charconv>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <sys/types.h>
#include <vector>
using json = nlohmann::json;

enum class CustomError { Ok, InvalidNumber, NoSuchTask, ParseError, IoError };
void printError(const CustomError &err);
class Task {
private:
  uint64_t id_;
  std::string text_;
  bool done_;

public:
  Task(uint64_t id, const std::string &text)
      : id_(id), text_(text), done_(false) {}
  Task(uint64_t id, const std::string &text, bool done)
      : id_(id), text_(text), done_(done) {}
  uint64_t getId() const { return id_; }
  std::string getText() const { return text_; }
  bool isDone() const { return done_; }
  void markAsDone() { done_ = true; }
};

class TaskManager {
private:
  std::vector<Task> tasks_;
  uint64_t nextId_;
  std::string filePath_;

public:
  TaskManager(const std::string &filePath)
      : tasks_({}), nextId_(1), filePath_(filePath) {
    printError(load());
  }

  void add(const std::string &text) {
    std::string taskText;

    if (text.empty()) {
      std::cout << "Enter task name: ";
      std::getline(std::cin, taskText);
      if (taskText.empty()) {
        std::cout << "No task provided.\n";
        return;
      }
      std::cout << std::endl;
    } else {
      taskText = text;
    }

    tasks_.push_back(Task(nextId_, text));
    nextId_++;
  }
  void ls(const std::string &flag = "") {
    if (tasks_.empty()) {
      std::cout << "Todo List is empty!\n";
      return;
    }

    std::vector<Task> tasksToShow;

    if (flag == "-d" || flag == "--done") {
      std::copy_if(tasks_.begin(), tasks_.end(),
                   std::back_inserter(tasksToShow),
                   [](const Task &task) { return task.isDone(); });
    } else if (flag == "-p" || flag == "--pending") {
      std::copy_if(tasks_.begin(), tasks_.end(),
                   std::back_inserter(tasksToShow),
                   [](const Task &task) { return !task.isDone(); });
    } else {
      tasksToShow = tasks_;
    }

    if (flag.find("-s") != std::string::npos ||
        flag.find("--sort") != std::string::npos) {
      if (flag.find("id") != std::string::npos) {
        std::sort(
            tasksToShow.begin(), tasksToShow.end(),
            [](const Task &i, const Task &j) { return i.getId() < j.getId(); });
      } else if (flag.find("done") != std::string::npos) {
        std::sort(tasksToShow.begin(), tasksToShow.end(),
                  [](const Task &i, const Task &j) {
                    return i.isDone() > j.isDone();
                  });
      }
    }
    int i = 1;
    for (const auto &task : tasksToShow) {
      std::cout << i++ << " [id=" << task.getId() << "]"
                << (task.isDone() ? " 🗹 " : " ☐ ");
      std::cout << task.getText() << std::endl;
    }
  };
  CustomError save(const std::string &path) {
    json root;
    root["next_id"] = nextId_;
    root["tasks"] = json::array();
    for (auto &task : tasks_) {
      json t;
      t["id"] = task.getId();
      t["text"] = task.getText();
      t["done"] = task.isDone();
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
  CustomError remove(uint64_t id) {
    auto index = findIndexById(id);
    if (!index) {
      return CustomError::NoSuchTask;
    }
    tasks_.erase(tasks_.begin() + *index);
    return CustomError::Ok;
  }
  CustomError markDone(uint64_t id) {
    auto index = findIndexById(id);
    if (!index) {
      return CustomError::NoSuchTask;
    }
    tasks_[*index].markAsDone();
    return CustomError::Ok;
  }

private:
  CustomError load() {
    std::ifstream file(filePath_);
    json data;
    tasks_.clear();
    uint64_t max = 0;

    if (!file) {
      return CustomError::Ok;
    }
    try {
      file >> data;
      data.at("next_id").get_to(nextId_);
      for (const auto &task : data.at("tasks")) {
        uint64_t id = task.at("id").get<uint64_t>();
        max = std::max(max, id);
        const std::string text = task.at("text").get<const std::string>();
        bool done = task.at("done").get<bool>();
        tasks_.push_back(Task(id, text, done));
      }
    } catch (const std::exception &e) {
      return CustomError::ParseError;
    }
    nextId_ = std::max(nextId_, max + 1);
    return CustomError::Ok;
  }
  std::optional<size_t> findIndexById(uint64_t id) const {
    auto it =
        std::find_if(tasks_.begin(), tasks_.end(),
                     [id](const Task &task) { return task.getId() == id; });
    if (it == tasks_.end()) {
      return std::nullopt;
    }
    return std::distance(tasks_.begin(), it);
  }
};
struct ResultIndex {
  CustomError code;
  size_t index = 0;
};
struct ResolvedId {
  CustomError code = CustomError::InvalidNumber;
  std::optional<uint64_t> id;
};

std::string trim(const std::string &userInput);
ResultIndex parseIndex(const size_t listSize, const std::string &userInput);

int main() {
  std::string userInput;
  std::string cmd;
  std::string flag;
  std::string path = "todo.json";

  TaskManager manager = TaskManager(path);

  std::cout << "Todo List." << std::endl;
  while (true) {
    std::cout << "Enter one of the following:\n"
                 "add [task] - to add any task\n"
                 "ls - to check current tasks\n"
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
      manager.add(flag);
      printError(manager.save(path));
    } else if (cmd == "ls") {
      manager.ls(flag);
    } else if (cmd == "done") {
      manager.markDone(parseIndex(manager.tasks));
    } else if (cmd == "del") {
      applyEditAndSave(state, flag, EditType::del, path);
    } else {
      std::cout << "Unknown command: " << cmd << std::endl;
    }
    std::cout << std::endl;
  }
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
