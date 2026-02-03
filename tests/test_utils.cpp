#include "../include/Utils.hpp"
#include "../include/catch.hpp"
#include <iostream>
#include <sstream>

namespace {
struct CoutCapture {
  std::streambuf *old = nullptr;
  std::ostringstream stream;
  CoutCapture() { old = std::cout.rdbuf(stream.rdbuf()); }
  ~CoutCapture() { std::cout.rdbuf(old); }
  std::string str() const { return stream.str(); }
};
} // namespace

TEST_CASE("stringToLower converts text", "[Utils]") {
  REQUIRE(stringToLower("AbC 123!") == "abc 123!");
}

TEST_CASE("trim removes leading and trailing whitespace", "[Utils]") {
  REQUIRE(trim("  hello  ") == "hello");
  REQUIRE(trim("\t hello\t") == "hello");
  REQUIRE(trim("   \t  ") == "");
}

TEST_CASE("printError writes expected messages", "[Utils]") {
  {
    CoutCapture capture;
    printError(CustomError::InvalidNumber);
    REQUIRE(capture.str() == "Error: Invalid number\n");
  }
  {
    CoutCapture capture;
    printError(CustomError::Ok);
    REQUIRE(capture.str().empty());
  }
}
