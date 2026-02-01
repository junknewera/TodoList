#include "../include/Command.hpp"
#include "../include/TaskManager.hpp"
#include <cstdint>
#include <optional>

AddCommand::AddCommand(TaskManager *manager, const std::string &text)
    : manager_(manager), text_(text) {}
CustomError AddCommand::execute() {
  std::optional<uint64_t> id = manager_->add(text_);
  if (id) {
    id_ = *id;
    return CustomError::Ok;
  }
  return CustomError::InvalidNumber;
}
CustomError AddCommand::undo() {
  auto isOk = manager_->removeById(id_);
  if (isOk) {
    return CustomError::Ok;
  }
  return CustomError::NoSuchTask;
}

DoneCommand::DoneCommand(TaskManager *manager, const std::string &text)
    : manager_(manager), text_(text) {}
CustomError DoneCommand::execute() {
  std::optional<bool> isDone = manager_->getTaskDoneStatus(text_);
  if (isDone == std::nullopt) {
    return CustomError::NoSuchTask;
  }
  previousDone_ = *isDone;
  return manager_->setTaskDone(text_, !*isDone);
}
CustomError DoneCommand::undo() {
  return manager_->setTaskDone(text_, previousDone_);
}

DelCommand::DelCommand(TaskManager *manager, const std::string &text)
    : manager_(manager), text_(text), task_(Task(0, "")) {}
CustomError DelCommand::execute() {
  auto task = manager_->removeTask(text_);
  if (task) {
    task_ = *task;
    return CustomError::Ok;
  }
  return CustomError::NoSuchTask;
}
CustomError DelCommand::undo() {
  auto isOk = manager_->addTask(task_);
  if (isOk) {
    return CustomError::Ok;
  }
  return CustomError::NoSuchTask;
}
