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
  task.markAsDone(true);

  REQUIRE(task.isDone() == true);
}

TEST_CASE("Task creation with done status", "[Task]") {
  Task task(1, "Test task", "general", Priority::low, true);

  REQUIRE(task.isDone() == true);
}

TEST_CASE("Task priority string matches enum", "[Task]") {
  Task lowTask(1, "Low task", "general", Priority::low, false);
  Task mediumTask(2, "Medium task", "general", Priority::medium, false);
  Task highTask(3, "High task", "general", Priority::high, false);

  REQUIRE(lowTask.getPriorityString() == "low");
  REQUIRE(mediumTask.getPriorityString() == "medium");
  REQUIRE(highTask.getPriorityString() == "high");
}
