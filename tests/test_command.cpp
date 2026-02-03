#include "../include/Command.hpp"
#include "../include/TaskManager.hpp"
#include "../include/catch.hpp"
#include "../include/json.hpp"
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>

using json = nlohmann::json;

namespace {
std::string makeTempPath(const std::string &tag) {
  static int counter = 0;
  return "/tmp/todo_test_" + tag + "_" + std::to_string(counter++) +
         ".json";
}

void removeFile(const std::string &path) { std::remove(path.c_str()); }

json loadJson(const std::string &path) {
  std::ifstream file(path);
  json data;
  file >> data;
  return data;
}
} // namespace

TEST_CASE("AddCommand executes and undo removes", "[Command]") {
  const std::string path = makeTempPath("addcmd");
  removeFile(path);

  TaskManager manager(path);
  auto cmd = std::make_unique<AddCommand>(manager, "Task one");
  REQUIRE(manager.executeCommand(std::move(cmd)) == CustomError::Ok);

  REQUIRE(manager.save(path) == CustomError::Ok);
  json data = loadJson(path);
  REQUIRE(data["tasks"].size() == 1);
  REQUIRE(data["tasks"][0]["text"].get<std::string>() == "Task one");

  manager.undo();
  REQUIRE(manager.save(path) == CustomError::Ok);
  json data2 = loadJson(path);
  REQUIRE(data2["tasks"].size() == 0);

  removeFile(path);
}

TEST_CASE("EditCommand undo restores previous text", "[Command]") {
  const std::string path = makeTempPath("editcmd");
  removeFile(path);

  TaskManager manager(path);
  auto id = manager.add("Original");
  REQUIRE(id.has_value());
  REQUIRE(*id == 1);

  auto cmd = std::make_unique<EditCommand>(manager, "1 Changed");
  REQUIRE(manager.executeCommand(std::move(cmd)) == CustomError::Ok);

  REQUIRE(manager.save(path) == CustomError::Ok);
  json data = loadJson(path);
  REQUIRE(data["tasks"][0]["text"].get<std::string>() == "Changed");

  manager.undo();
  REQUIRE(manager.save(path) == CustomError::Ok);
  json data2 = loadJson(path);
  REQUIRE(data2["tasks"][0]["text"].get<std::string>() == "Original");

  removeFile(path);
}

TEST_CASE("DoneCommand and UndoneCommand toggle status with undo", "[Command]") {
  const std::string path = makeTempPath("donecmd");
  removeFile(path);

  TaskManager manager(path);
  auto id = manager.add("Task");
  REQUIRE(id.has_value());
  REQUIRE(*id == 1);

  auto doneCmd = std::make_unique<DoneCommand>(manager, "1");
  REQUIRE(manager.executeCommand(std::move(doneCmd)) == CustomError::Ok);
  REQUIRE(manager.getTaskDoneStatus("1").value() == true);

  manager.undo();
  REQUIRE(manager.getTaskDoneStatus("1").value() == false);

  REQUIRE(manager.setTaskDone("1", true) == CustomError::Ok);
  auto undoneCmd = std::make_unique<UndoneCommand>(manager, "1");
  REQUIRE(manager.executeCommand(std::move(undoneCmd)) == CustomError::Ok);
  REQUIRE(manager.getTaskDoneStatus("1").value() == false);

  manager.undo();
  REQUIRE(manager.getTaskDoneStatus("1").value() == true);

  removeFile(path);
}

TEST_CASE("DelCommand undo restores task order", "[Command]") {
  const std::string path = makeTempPath("delcmd");
  removeFile(path);

  TaskManager manager(path);
  auto id1 = manager.add("First");
  REQUIRE(id1.has_value());
  REQUIRE(*id1 == 1);
  auto id2 = manager.add("Second");
  REQUIRE(id2.has_value());
  REQUIRE(*id2 == 2);

  auto cmd = std::make_unique<DelCommand>(manager, "1");
  REQUIRE(manager.executeCommand(std::move(cmd)) == CustomError::Ok);

  REQUIRE(manager.save(path) == CustomError::Ok);
  json data = loadJson(path);
  REQUIRE(data["tasks"].size() == 1);
  REQUIRE(data["tasks"][0]["text"].get<std::string>() == "Second");

  manager.undo();
  REQUIRE(manager.save(path) == CustomError::Ok);
  json data2 = loadJson(path);
  REQUIRE(data2["tasks"].size() == 2);
  REQUIRE(data2["tasks"][0]["text"].get<std::string>() == "First");
  REQUIRE(data2["tasks"][1]["text"].get<std::string>() == "Second");

  removeFile(path);
}

TEST_CASE("ClearCommand undo restores tasks", "[Command]") {
  const std::string path = makeTempPath("clearcmd");
  removeFile(path);

  TaskManager manager(path);
  auto id1 = manager.add("Alpha");
  REQUIRE(id1.has_value());
  REQUIRE(*id1 == 1);
  auto id2 = manager.add("Beta");
  REQUIRE(id2.has_value());
  REQUIRE(*id2 == 2);

  auto cmd = std::make_unique<ClearCommand>(manager);
  REQUIRE(manager.executeCommand(std::move(cmd)) == CustomError::Ok);

  REQUIRE(manager.save(path) == CustomError::Ok);
  json data = loadJson(path);
  REQUIRE(data["tasks"].size() == 0);

  manager.undo();
  REQUIRE(manager.save(path) == CustomError::Ok);
  json data2 = loadJson(path);
  REQUIRE(data2["tasks"].size() == 2);

  removeFile(path);
}

TEST_CASE("executeCommand does not push failed commands", "[Command]") {
  const std::string path = makeTempPath("failed");
  removeFile(path);

  TaskManager manager(path);
  auto id = manager.add("Only task");
  REQUIRE(id.has_value());
  REQUIRE(*id == 1);

  auto cmd = std::make_unique<DelCommand>(manager, "2");
  REQUIRE(manager.executeCommand(std::move(cmd)) == CustomError::NoSuchTask);

  manager.undo();
  REQUIRE(manager.save(path) == CustomError::Ok);
  json data = loadJson(path);
  REQUIRE(data["tasks"].size() == 1);
  REQUIRE(data["tasks"][0]["text"].get<std::string>() == "Only task");

  removeFile(path);
}
