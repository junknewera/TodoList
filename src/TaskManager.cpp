#include "../include/TaskManager.hpp"
#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>

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
std::string trim(const std::string &userInput) {
  const auto first = userInput.find_first_not_of(" \t");
  if (first == std::string::npos) {
    return "";
  }
  const auto last = userInput.find_last_not_of(" \t");
  return userInput.substr(first, last - first + 1);
}
TaskManager::TaskManager(const std::string &filePath)
    : tasks_(), nextId_(1), filePath_(filePath) {
  printError(load());
}
void TaskManager::add(const std::string &text) {
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

  tasks_.push_back(Task(nextId_, taskText));
  nextId_++;
}
void TaskManager::ls(const std::string &flag) const {
  if (tasks_.empty()) {
    std::cout << "Todo List is empty!\n";
    return;
  }

  std::vector<Task> tasksToShow;

  if (flag == "-d" || flag == "--done") {
    std::copy_if(tasks_.begin(), tasks_.end(), std::back_inserter(tasksToShow),
                 [](const Task &task) { return task.isDone(); });
  } else if (flag == "-p" || flag == "--pending") {
    std::copy_if(tasks_.begin(), tasks_.end(), std::back_inserter(tasksToShow),
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
      std::sort(
          tasksToShow.begin(), tasksToShow.end(),
          [](const Task &i, const Task &j) { return i.isDone() > j.isDone(); });
    }
  }
  int i = 1;
  for (const auto &task : tasksToShow) {
    std::cout << i++ << " [id=" << task.getId() << "]"
              << (task.isDone() ? " 🗹 " : " ☐ ");
    std::cout << task.getText() << std::endl;
  }
};
CustomError TaskManager::save(const std::string &path) {
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
CustomError TaskManager::remove(const std::string &flag) {
  ResolvedId rid = resolveIdFromUserNumber(flag);
  if (rid.code != CustomError::Ok) {
    return rid.code;
  }
  std::optional<uint64_t> index = findIndexById(*rid.id);

  if (!index) {
    return CustomError::NoSuchTask;
  }
  tasks_.erase(tasks_.begin() + *index);
  return CustomError::Ok;
}
CustomError TaskManager::markDone(const std::string &flag) {
  ResolvedId rid = resolveIdFromUserNumber(flag);
  if (rid.code != CustomError::Ok) {
    return rid.code;
  }
  std::optional<uint64_t> index = findIndexById(*rid.id);

  if (!index) {
    return CustomError::NoSuchTask;
  }
  tasks_[*index].markAsDone();
  return CustomError::Ok;
}
void TaskManager::printHelp() const {

  std::cout <<
      R"(Todo List commands:

help
    Show usage

add <task>
    Add new task

ls [options]
    -s, --sort <id|done>   Sort tasks
    -p, --pending          Show only pending tasks
    -d, --done             Show only completed tasks

done <id>
    Mark task as done

del <id>
    Delete task

q
    Quit
)";
}
CustomError TaskManager::load() {
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
std::optional<size_t> TaskManager::findIndexById(uint64_t id) const {
  auto it = std::find_if(tasks_.begin(), tasks_.end(),
                         [id](const Task &task) { return task.getId() == id; });
  if (it == tasks_.end()) {
    return std::nullopt;
  }
  return std::distance(tasks_.begin(), it);
}
ResolvedId TaskManager::resolveIdFromUserNumber(const std::string &flag) {
  std::string input = flag;
  if (flag.empty()) {
    std::cout << "Enter task number: ";
    std::getline(std::cin, input);
  }
  input = trim(input);

  ResultIndex index = parseIndex(input);
  if (index.code != CustomError::Ok) {
    return {index.code, std::nullopt};
  }
  return {CustomError::Ok, tasks_[index.index].getId()};
}
ResultIndex TaskManager::parseIndex(const std::string &userInput) {
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
  if (num < 1 || static_cast<size_t>(num) > tasks_.size()) {
    index.code = CustomError::NoSuchTask;
    return index;
  }
  index.code = CustomError::Ok;
  index.index = static_cast<size_t>(num - 1);
  return index;
}
