#include "../include/Utils.hpp"
#include <algorithm>
#include <iostream>

std::string stringToLower(const std::string &text) {
  std::string lowerText(text.size(), '\0');
  std::transform(text.begin(), text.end(), lowerText.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return lowerText;
}
void printError(const CustomError &err) {
  switch (err) {
  case CustomError::Ok:
    return;
  case CustomError::InvalidNumber:
    std::cout << "Error: Invalid number\n";
    return;
  case CustomError::NoSuchTask:
    std::cout << "Error: No such task\n";
    return;
  case CustomError::ParseError:
    std::cout << "Error: Parse Error\n";
    return;
  case CustomError::IoError:
    std::cout << "Error: Input/Output Error\n";
    return;
  }
}
std::string trim(const std::string &userInput) {
  const auto first = userInput.find_first_not_of(" \t");
  if (first == std::string::npos) {
    return "";
  }
  const auto last = userInput.find_last_not_of(" \t");
  return userInput.substr(first, last - first + 1);
}
