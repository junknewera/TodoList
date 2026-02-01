#pragma once
#include <cstdint>
#include <string>

enum class Priority { low = 0, medium = 1, high = 2 };
class Task {
private:
  uint64_t id_;
  std::string text_;
  std::string category_;
  Priority priority_;
  bool done_;

public:
  Task(uint64_t id, const std::string &text,
       const std::string &category = "general",
       const Priority &priority = Priority::low);
  Task(uint64_t id, const std::string &text, const std::string &category,
       const Priority &priority, bool done);
  uint64_t getId() const { return id_; }
  std::string getText() const { return text_; }
  std::string getCategory() const { return category_; }
  Priority getPriority() const { return priority_; }
  std::string getPriorityString() const;
  bool isDone() const { return done_; }
  void markAsDone(bool done) { done_ = done; }
};
