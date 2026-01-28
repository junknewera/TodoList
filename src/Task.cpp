#include "../include/Task.hpp"
Task::Task(uint64_t id, const std::string &text)
    : id_(id), text_(text), done_(false) {}
Task::Task(uint64_t id, const std::string &text, bool done)
    : id_(id), text_(text), done_(done) {}
