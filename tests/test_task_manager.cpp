#include "../include/TaskManager.hpp"
#include "../include/catch.hpp"
#include "../include/json.hpp"
#include <cstdio>
#include <fstream>
#include <string>

using json = nlohmann::json;

namespace {
std::string makeTempPath(const std::string &tag) {
  static int counter = 0;
  return "/tmp/todo_test_" + tag + "_" + std::to_string(counter++) + ".json";
}

void removeFile(const std::string &path) { std::remove(path.c_str()); }

json loadJson(const std::string &path) {
  std::ifstream file(path);
  json data;
  file >> data;
  return data;
}
} // namespace

TEST_CASE("TaskManager add parses category and priority", "[TaskManager]") {
  const std::string path = makeTempPath("add");
  removeFile(path);

  TaskManager manager(path);

  auto id1 = manager.add("work:high:Finish report");
  REQUIRE(id1.has_value());
  REQUIRE(*id1 == 1);
  auto id2 = manager.add("home:Clean");
  REQUIRE(id2.has_value());
  REQUIRE(*id2 == 2);
  auto id3 = manager.add("Plain task");
  REQUIRE(id3.has_value());
  REQUIRE(*id3 == 3);
  REQUIRE(manager.save(path) == CustomError::Ok);

  json data = loadJson(path);
  REQUIRE(data["next_id"].get<uint64_t>() == 4);
  REQUIRE(data["tasks"].size() == 3);

  json t0 = data["tasks"][0];
  REQUIRE(t0["id"].get<uint64_t>() == 1);
  REQUIRE(t0["text"].get<std::string>() == "Finish report");
  REQUIRE(t0["category"].get<std::string>() == "work");
  REQUIRE(t0["priority"].get<int>() == static_cast<int>(Priority::high));
  REQUIRE(t0["done"].get<bool>() == false);

  json t1 = data["tasks"][1];
  REQUIRE(t1["id"].get<uint64_t>() == 2);
  REQUIRE(t1["text"].get<std::string>() == "Clean");
  REQUIRE(t1["category"].get<std::string>() == "home");
  REQUIRE(t1["priority"].get<int>() == static_cast<int>(Priority::low));

  json t2 = data["tasks"][2];
  REQUIRE(t2["id"].get<uint64_t>() == 3);
  REQUIRE(t2["text"].get<std::string>() == "Plain task");
  REQUIRE(t2["category"].get<std::string>() == "general");
  REQUIRE(t2["priority"].get<int>() == static_cast<int>(Priority::low));

  removeFile(path);
}

TEST_CASE("TaskManager uses low priority for invalid input", "[TaskManager]") {
  const std::string path = makeTempPath("priority");
  removeFile(path);

  TaskManager manager(path);
  auto id = manager.add("work:urgent:Something");
  REQUIRE(id.has_value());
  REQUIRE(*id == 1);
  REQUIRE(manager.save(path) == CustomError::Ok);

  json data = loadJson(path);
  REQUIRE(data["tasks"][0]["priority"].get<int>() ==
          static_cast<int>(Priority::low));

  removeFile(path);
}

TEST_CASE("TaskManager edit updates text", "[TaskManager]") {
  const std::string path = makeTempPath("edit");
  removeFile(path);

  TaskManager manager(path);
  auto id = manager.add("First task");
  REQUIRE(id.has_value());
  REQUIRE(*id == 1);

  auto result = manager.editTask("1 Updated task");
  REQUIRE(result.first.has_value());
  REQUIRE(result.second.has_value());
  REQUIRE(*result.second == "First task");

  REQUIRE(manager.save(path) == CustomError::Ok);
  json data = loadJson(path);
  REQUIRE(data["tasks"][0]["text"].get<std::string>() == "Updated task");

  removeFile(path);
}

TEST_CASE("TaskManager markDone and undone update status", "[TaskManager]") {
  const std::string path = makeTempPath("done");
  removeFile(path);

  TaskManager manager(path);
  auto id = manager.add("Task");
  REQUIRE(id.has_value());
  REQUIRE(*id == 1);

  REQUIRE(manager.markDone("1") == CustomError::Ok);
  auto status = manager.getTaskDoneStatus("1");
  REQUIRE(status.has_value());
  REQUIRE(*status == true);

  REQUIRE(manager.undone("1") == CustomError::Ok);
  status = manager.getTaskDoneStatus("1");
  REQUIRE(status.has_value());
  REQUIRE(*status == false);

  REQUIRE(manager.markDone("2") == CustomError::NoSuchTask);
  REQUIRE(!manager.getTaskDoneStatus("2").has_value());

  removeFile(path);
}

TEST_CASE("TaskManager remove, removeTask, insertByIndex", "[TaskManager]") {
  const std::string path = makeTempPath("remove");
  removeFile(path);

  TaskManager manager(path);
  auto id1 = manager.add("First");
  REQUIRE(id1.has_value());
  REQUIRE(*id1 == 1);
  auto id2 = manager.add("Second");
  REQUIRE(id2.has_value());
  REQUIRE(*id2 == 2);

  REQUIRE(manager.remove("2") == CustomError::Ok);
  REQUIRE(manager.save(path) == CustomError::Ok);
  json data = loadJson(path);
  REQUIRE(data["tasks"].size() == 1);
  REQUIRE(data["tasks"][0]["text"].get<std::string>() == "First");

  auto removed = manager.removeTask("1");
  REQUIRE(removed.first.has_value());
  REQUIRE(removed.second.has_value());
  REQUIRE(removed.first->getText() == "First");

  auto restored = manager.insertByIndex(*removed.first, *removed.second);
  REQUIRE(restored.has_value());

  REQUIRE(manager.save(path) == CustomError::Ok);
  json data2 = loadJson(path);
  REQUIRE(data2["tasks"].size() == 1);
  REQUIRE(data2["tasks"][0]["text"].get<std::string>() == "First");

  removeFile(path);
}

TEST_CASE("TaskManager clear and load tasks", "[TaskManager]") {
  const std::string path = makeTempPath("clear");
  removeFile(path);

  TaskManager manager(path);
  auto id1 = manager.add("Alpha");
  REQUIRE(id1.has_value());
  REQUIRE(*id1 == 1);
  auto id2 = manager.add("Beta");
  REQUIRE(id2.has_value());
  REQUIRE(*id2 == 2);

  auto previous = manager.clearTasks();
  REQUIRE(manager.save(path) == CustomError::Ok);
  json data = loadJson(path);
  REQUIRE(data["tasks"].size() == 0);

  manager.loadTasks(previous);
  REQUIRE(manager.save(path) == CustomError::Ok);
  json data2 = loadJson(path);
  REQUIRE(data2["tasks"].size() == 2);

  removeFile(path);
}

TEST_CASE("TaskManager save returns IoError on invalid path", "[TaskManager]") {
  const std::string path = makeTempPath("save");
  removeFile(path);

  TaskManager manager(path);
  REQUIRE(manager.save("/tmp") == CustomError::IoError);

  removeFile(path);
}
