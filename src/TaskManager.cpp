#include "../include/TaskManager.hpp"
#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>

std::string stringToLower(const std::string &text) {
  std::string lowerText(text.size(), '\0');
  std::transform(text.begin(), text.end(), lowerText.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return lowerText;
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
CustomError TaskManager::executeCommand(std::unique_ptr<Command> command) {
  command->execute();
  stack_.push(std::move(command));
  return CustomError::Ok;
}
void TaskManager::undo() {
  if (stack_.empty()) {
    std::cout << "Nothing to do\n";
    return;
  }
  stack_.top()->undo();
  stack_.pop();
}

TaskManager::~TaskManager() = default;
std::optional<uint64_t> TaskManager::add(const std::string &text) {
  std::string taskText;

  if (text.empty()) {
    std::cout << "Enter task name: ";
    std::getline(std::cin, taskText);
    if (taskText.empty()) {
      std::cout << "No task provided.\n";
      return {};
    }
    std::cout << std::endl;
  } else {
    taskText = text;
  }
  size_t delimiter = taskText.find(":");
  if (delimiter != std::string::npos) {
    std::string category = taskText.substr(0, delimiter);
    std::string text = taskText.substr(delimiter + 1, taskText.size());

    size_t delimCat = text.find(":");
    if (delimCat != std::string::npos) {
      Priority priority;
      std::string prior = text.substr(0, delimCat);
      if (prior == "low") {
        priority = Priority::low;
      } else if (prior == "medium") {
        priority = Priority::medium;
      } else if (prior == "high") {
        priority = Priority::high;
      } else {
        std::cout << "Wrong priority, claims low\n";
        priority = Priority::low;
      }
      std::string txt = text.substr(delimCat + 1, text.size());
      tasks_.push_back(Task(nextId_, txt, category, priority));
    } else {
      tasks_.push_back(Task(nextId_, text, category));
    }
  } else {
    tasks_.push_back(Task(nextId_, taskText));
  }
  return nextId_++;
}
std::optional<uint64_t> TaskManager::addTask(const Task &task) {
  if (task.getId() != 0) {
    tasks_.push_back(task);
    return task.getId();
  }
  return {};
}

std::optional<uint64_t> TaskManager::removeById(uint64_t id) {
  std::optional<size_t> index = findIndexById(id);
  if (index) {
    tasks_.erase(tasks_.begin() + *index);
    return id;
  }
  return {};
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
  } else if (flag == "-l" || flag == "--low") {
    std::copy_if(
        tasks_.begin(), tasks_.end(), std::back_inserter(tasksToShow),
        [](const Task &task) { return task.getPriority() == Priority::low; });
  } else if (flag == "-m" || flag == "--medium") {
    std::copy_if(tasks_.begin(), tasks_.end(), std::back_inserter(tasksToShow),
                 [](const Task &task) {
                   return task.getPriority() == Priority::medium;
                 });
  } else if (flag == "-h" || flag == "--high") {
    std::copy_if(
        tasks_.begin(), tasks_.end(), std::back_inserter(tasksToShow),
        [](const Task &task) { return task.getPriority() == Priority::high; });
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
    } else if (flag.find("priority")) {
      std::sort(tasksToShow.begin(), tasksToShow.end(),
                [](const Task &i, const Task &j) {
                  return i.getPriority() < j.getPriority();
                });
    }
  }

  std::vector<Task> foundTasks;
  if (flag.find("-f") != std::string::npos ||
      flag.find("--find") != std::string::npos) {
    size_t delimiter = flag.find(" ");
    if (delimiter != std::string::npos) {
      std::string textToSearch = flag.substr(delimiter + 1);
      std::copy_if(tasksToShow.begin(), tasksToShow.end(),
                   std::back_inserter(foundTasks),
                   [&textToSearch](const Task &task) {
                     return stringToLower(task.getText()).find(textToSearch) !=
                            std::string::npos;
                   });
      tasksToShow = foundTasks;
    }
  }

  int i = 1;

  for (const auto &task : tasksToShow) {
    std::cout << i++ << " [id=" << task.getId() << "]" << " ["
              << task.getCategory() << "] " << "[" << task.getPriorityString()
              << "] " << (task.isDone() ? " 🗹 " : " ☐ ");
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
    t["category"] = task.getCategory();
    t["priority"] = task.getPriority();
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
std::optional<Task> TaskManager::removeTask(const std::string &flag) {
  ResolvedId rid = resolveIdFromUserNumber(flag);
  if (rid.code != CustomError::Ok) {
    return {};
  }
  std::optional<uint64_t> index = findIndexById(*rid.id);

  if (!index) {
    return {};
  }
  Task taskToReturn = tasks_[*index];
  tasks_.erase(tasks_.begin() + *index);
  return taskToReturn;
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
  tasks_[*index].markAsDone(true);
  return CustomError::Ok;
}
std::optional<bool> TaskManager::getTaskDoneStatus(const std::string &flag) {
  ResolvedId rid = resolveIdFromUserNumber(flag);
  if (rid.code != CustomError::Ok) {
    return {};
  }
  std::optional<uint64_t> index = findIndexById(*rid.id);

  if (!index) {
    return {};
  }
  return tasks_[*index].isDone();
}
CustomError TaskManager::setTaskDone(const std::string &flag, bool done) {
  ResolvedId rid = resolveIdFromUserNumber(flag);
  if (rid.code != CustomError::Ok) {
    return rid.code;
  }
  std::optional<uint64_t> index = findIndexById(*rid.id);

  if (!index) {
    return CustomError::NoSuchTask;
  }
  tasks_[*index].markAsDone(done);
  return CustomError::Ok;
}
void TaskManager::printHelp() const {

  std::cout <<
      R"(Todo List commands:

help
    Show usage

add <category:priority:task>
    Add new task with category and priority

ls [options]
    -s, --sort <id|done|priority>   Sort tasks
    -p, --pending                   Show only pending tasks
    -d, --done                      Show only completed tasks
    -l, --low                       Show only low priority tasks
    -m, --medium                    Show only medium priority tasks
    -h, --high                      Show only high priority tasks

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
      const std::string category = task.at("category").get<const std::string>();
      const Priority priority = task.at("priority").get<const Priority>();
      bool done = task.at("done").get<bool>();
      tasks_.push_back(Task(id, text, category, priority, done));
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
