#pragma once
#include "Command.hpp"
#include "Task.hpp"
#include "json.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <vector>
using json = nlohmann::json;

enum class CustomError { Ok, InvalidNumber, NoSuchTask, ParseError, IoError };
struct ResultIndex {
  CustomError code;
  size_t index = 0;
};
struct ResolvedId {
  CustomError code = CustomError::InvalidNumber;
  std::optional<uint64_t> id;
};
class TaskManager {
private:
  std::vector<Task> tasks_;
  uint64_t nextId_;
  std::string filePath_;
  std::stack<std::unique_ptr<Command>> stack_;

public:
  TaskManager(const std::string &filePath);
  ~TaskManager();

  std::optional<uint64_t> add(const std::string &text);
  std::optional<uint64_t> insertByIndex(const Task &task, size_t index);
  void ls(const std::string &flag = "") const;
  CustomError save(const std::string &path);
  CustomError remove(const std::string &flag);
  std::optional<uint64_t> removeById(uint64_t id);
  CustomError markDone(const std::string &flag);
  void printHelp() const;
  CustomError executeCommand(std::unique_ptr<Command> command);
  std::optional<bool> getTaskDoneStatus(const std::string &flag);
  CustomError setTaskDone(const std::string &flag, bool done);
  std::pair<std::optional<Task>, std::optional<size_t>>
  removeTask(const std::string &flag);
  void undo();

private:
  CustomError load();
  std::optional<size_t> findIndexById(uint64_t id) const;
  ResolvedId resolveIdFromUserNumber(const std::string &flag);
  ResultIndex parseIndex(const std::string &userInput);
};
