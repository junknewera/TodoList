#pragma once
#include "Task.hpp"
#include <cstdint>
#include <string>
#include <vector>

enum class CustomError;
class TaskManager;
enum class Priority;

class Command {
public:
  virtual ~Command() = default;
  virtual CustomError execute() = 0;
  virtual CustomError undo() = 0;
};
class AddCommand : public Command {
private:
  TaskManager &manager_;
  uint64_t id_;
  std::string text_;

public:
  AddCommand(TaskManager &manager, const std::string &text);
  CustomError execute() override;
  CustomError undo() override;
};
class EditCommand : public Command {
private:
  TaskManager &manager_;
  size_t idx_;
  std::string text_;
  std::string previousText_;

public:
  EditCommand(TaskManager &manager, const std::string &text);
  CustomError execute() override;
  CustomError undo() override;
};
class DoneCommand : public Command {
private:
  TaskManager &manager_;
  const std::string text_;
  bool previousDone_;

public:
  DoneCommand(TaskManager &manager, const std::string &text);
  CustomError execute() override;
  CustomError undo() override;
};
class UndoneCommand : public Command {
private:
  TaskManager &manager_;
  const std::string text_;
  bool previousDone_;

public:
  UndoneCommand(TaskManager &manager, const std::string &text);
  CustomError execute() override;
  CustomError undo() override;
};
class DelCommand : public Command {
private:
  TaskManager &manager_;
  std::string text_;
  Task task_;
  size_t index_;

public:
  DelCommand(TaskManager &manager, const std::string &text);
  CustomError execute() override;
  CustomError undo() override;
};
class ClearCommand : public Command {
private:
  TaskManager &manager_;
  std::vector<Task> previousTasks_;

public:
  ClearCommand(TaskManager &manager);
  CustomError execute() override;
  CustomError undo() override;
};
