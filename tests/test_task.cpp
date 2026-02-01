#define CATCH_CONFIG_MAIN
#include "../include/Task.hpp"
#include "../include/catch.hpp"

TEST_CASE("Task creation", "[Task]") {
  Task task(1, "Test task");

  REQUIRE(task.getId() == 1);
  REQUIRE(task.getText() == "Test task");
  REQUIRE(task.isDone() == false);
}

TEST_CASE("Task mark as done", "[Task]") {
  Task task(1, "Test task");
  task.markAsDone();

  REQUIRE(task.isDone() == true);
}

TEST_CASE("Task creation with done status", "[Task]") {
  Task task(1, "Test task", true);

  REQUIRE(task.isDone() == true);
}
