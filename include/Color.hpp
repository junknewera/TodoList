#pragma once
#include <ostream>
#include <string>

class Color {
private:
  std::ostream &os_;

public:
  Color(std::ostream &os, const std::string &color) : os_(os) {
    os_ << "\033[" << color << "m";
  };
  ~Color() { os_ << "\033[0m"; }
};
