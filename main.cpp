#include <cctype>
#include <iostream>
#include <vector>

void add(std::vector<std::string> &todoList);
void list(std::vector<std::string> &todoList);
int main() {
  std::vector<std::string> todoList{};
  std::string userInput;

  std::cout << "Todo List." << std::endl;
  while (true) {
    std::cout << "Enter one of the following: " << std::endl;
    std::cout << "add - to add any task" << std::endl;
    std::cout << "list - to check current tasks" << std::endl;
    std::cout << "q - to quit" << std::endl;

    std::getline(std::cin, userInput);
    if (userInput == "q") {
      break;
    } else if (userInput == "add") {
      add(todoList);
    } else if (userInput == "list") {
      list(todoList);
    }
  }
}

void add(std::vector<std::string> &todoList) {
  std::string userInput;
  std::cout << "Enter task name: ";
  std::getline(std::cin, userInput);
  std::cout << std::endl;

  todoList.push_back(userInput);
}
void list(std::vector<std::string> &todoList) {
  if (todoList.empty()) {
    std::cout << "Todo List is empty!\n";
  } else {
    for (int i = 0; i < todoList.size(); i++) {
      std::cout << i + 1 << ". " << todoList[i] << std::endl;
    }
  }
}
