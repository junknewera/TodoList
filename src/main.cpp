#include "../include/Command.hpp"
#include "../include/TaskManager.hpp"
#include <iostream>
#include <memory>
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
      auto command = std::make_unique<AddCommand>(&manager, flag);
      manager.executeCommand(std::move(command));
      printError(manager.save(path));
    } else if (cmd == "undo") {
      manager.undo();
      manager.save(path);
    } else if (cmd == "ls") {
      manager.ls(flag);
    } else if (cmd == "done") {
      auto command = std::make_unique<DoneCommand>(&manager, flag);
      printError(manager.executeCommand(std::move(command)));
      printError(manager.save(path));
    } else if (cmd == "del") {
      auto command = std::make_unique<DelCommand>(&manager, flag);
      printError(manager.executeCommand(std::move(command)));
      printError(manager.save(path));
    } else {
      std::cout << "Unknown command: " << cmd << std::endl;
    }
    std::cout << std::endl;
  }
}
