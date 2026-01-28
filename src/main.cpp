#include "../include/TaskManager.hpp"
#include <iostream>
#include <string>

void printError(const CustomError &err);
std::string trim(const std::string &userInput);

int main() {
  std::string userInput;
  std::string cmd;
  std::string flag;
  std::string path = "todo.json";

  TaskManager manager = TaskManager(path);

  std::cout << "Todo List." << std::endl;
  manager.printHelp();
  while (true) {
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
    } else if (cmd == "help") {
      manager.printHelp();
    } else if (cmd == "add") {
      manager.add(flag);
      printError(manager.save(path));
    } else if (cmd == "ls") {
      manager.ls(flag);
    } else if (cmd == "done") {
      printError(manager.markDone(flag));
      manager.save(path);
    } else if (cmd == "del") {
      printError(manager.remove(flag));
      manager.save(path);
    } else {
      std::cout << "Unknown command: " << cmd << std::endl;
    }
    std::cout << std::endl;
  }
}
