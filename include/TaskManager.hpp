#pragma once
#include "../json.hpp"
#include "Task.hpp"
#include <optional>
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

public:
  TaskManager(const std::string &filePath);

  void add(const std::string &text);
  void ls(const std::string &flag = "") const;
  CustomError save(const std::string &path);
  CustomError remove(const std::string &flag);
  CustomError markDone(const std::string &flag);
  void printHelp() const;

private:
  CustomError load();
  std::optional<size_t> findIndexById(uint64_t id) const;
  ResolvedId resolveIdFromUserNumber(const std::string &flag);
  ResultIndex parseIndex(const std::string &userInput);
};
