#include "../include/Task.hpp"
std::string Task::getPriorityString() const {
  if (priority_ == Priority::low) {
    return "low";
  } else if (priority_ == Priority::medium) {
    return "medium";
  } else if (priority_ == Priority::high) {
    return "high";
  }
  return "low";
}
Task::Task(uint64_t id, const std::string &text, const std::string &category,
           const Priority priority, bool done)
    : id_(id), text_(text), category_(category), priority_(priority),
      done_(done) {}
