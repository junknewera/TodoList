#pragma once
#include <cstdint>
#include <string>

class Task {
private:
  uint64_t id_;
  std::string text_;
  bool done_;

public:
  Task(uint64_t id, const std::string &text);
  Task(uint64_t id, const std::string &text, bool done);
  uint64_t getId() const { return id_; }
  std::string getText() const { return text_; }
  bool isDone() const { return done_; }
  void markAsDone() { done_ = true; }
};
