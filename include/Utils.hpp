#pragma once
#include <cstdint>
#include <optional>
#include <string>

enum class CustomError { Ok, InvalidNumber, NoSuchTask, ParseError, IoError };
struct ResultIndex {
  CustomError code;
  size_t index = 0;
};
struct ResolvedId {
  CustomError code = CustomError::InvalidNumber;
  std::optional<uint64_t> id;
};
std::string stringToLower(const std::string &text);
void printError(const CustomError &err);
std::string trim(const std::string &userInput);
