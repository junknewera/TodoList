#include <ios>
#include <iostream>
#include <limits>
#include <vector>

struct Task {
  std::string text;
  bool done = false;
};

void add(std::vector<Task> &todoList);
void list(const std::vector<Task> &todoList);
void done(std::vector<Task> &todoList);
void remove(std::vector<Task> &todoList);

int main() {
  std::vector<Task> todoList{};
  std::string userInput;

  std::cout << "Todo List." << std::endl;
  while (true) {
    std::cout << "Enter one of the following:\n"
                 "add - to add any task\n"
                 "list - to check current tasks\n"
                 "done - to mark any task as done\n"
                 "del - to remove any task\n"
                 "q - to quit\n";
    std::getline(std::cin, userInput);
    if (userInput == "q") {
      break;
    } else if (userInput == "add") {
      add(todoList);
    } else if (userInput == "list") {
      list(todoList);
    } else if (userInput == "done") {
      list(todoList);
      done(todoList);
    } else if (userInput == "del") {
      list(todoList);
      remove(todoList);
    } else {
      std::cout << "Unknown command: " << userInput << std::endl;
    }
    std::cout << std::endl;
  }
}

void add(std::vector<Task> &todoList) {
  Task task;

  std::cout << "Enter task name: ";
  std::getline(std::cin, task.text);
  if (task.text.empty()) {
    std::cout << "No task provided.\n";
    return;
  }
  std::cout << std::endl;

  todoList.push_back(task);
}
void list(const std::vector<Task> &todoList) {
  if (todoList.empty()) {
    std::cout << "Todo List is empty!\n";
  } else {
    int i = 1;
    for (auto &task : todoList) {
      std::cout << i++ << (task.done ? " 🗹 " : " ☐ ");
      std::cout << task.text << std::endl;
    }
  }
}
void done(std::vector<Task> &todoList) {
  int taskNum;
  std::cout << "Enter task number: ";
  std::cin >> taskNum;
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  if (taskNum < 1 || static_cast<size_t>(taskNum) > todoList.size()) {
    std::cout << "No such task";
    return;
  }
  todoList[taskNum - 1].done = true;
  std::cout << std::endl;
}
void remove(std::vector<Task> &todoList) {
  int taskNum;
  std::cout << "Enter task number: ";
  std::cin >> taskNum;
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  if (taskNum < 1 || static_cast<size_t>(taskNum) > todoList.size()) {
    std::cout << "No such task";
    return;
  }
  todoList.erase(todoList.begin() + taskNum - 1);
  std::cout << std::endl;
}
