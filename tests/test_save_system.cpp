#include "SaveSystem.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <system_error>

namespace {
int failures = 0;
void check(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
    ++failures;
  }
}

void writeText(const std::filesystem::path& path, const std::string& value) {
  std::ofstream output(path, std::ios::trunc);
  output << value;
}
}  // namespace

int main() {
  using namespace space_survivor;
  const auto testDirectory = std::filesystem::temp_directory_path() /
                               "space_survivor_save_system_test";
  std::error_code error;
  std::filesystem::remove_all(testDirectory, error);
  const auto savePath = testDirectory / "highscore.txt";
  const SaveSystem saves(savePath);

  check(saves.loadHighScore() == 0, "missing file loads as zero");
  check(saves.saveHighScore(4250), "save writes a high score");
  check(saves.loadHighScore() == 4250, "saved value round-trips");
  check(saves.saveHighScore(5200), "existing save is replaced");
  check(saves.loadHighScore() == 5200, "replacement is readable");

  writeText(savePath, "");
  check(saves.loadHighScore() == 0, "empty file loads as zero");
  writeText(savePath, "-1\n");
  check(saves.loadHighScore() == 0, "negative value loads as zero");
  writeText(savePath, "not-a-number\n");
  check(saves.loadHighScore() == 0, "invalid text loads as zero");
  writeText(savePath, "123 trailing\n");
  check(saves.loadHighScore() == 0, "trailing text invalidates the record");
  writeText(savePath, "2147483648\n");
  check(saves.loadHighScore() == 0, "value above INT_MAX is rejected");

  const int maxScore = std::numeric_limits<int>::max();
  writeText(savePath, std::to_string(maxScore) + "\n");
  check(saves.loadHighScore() == maxScore, "INT_MAX remains a valid score");
  check(saves.saveHighScore(-100), "negative input is normalized on save");
  check(saves.loadHighScore() == 0, "negative input saves as zero");

  std::filesystem::remove_all(testDirectory, error);
  return failures == 0 ? 0 : 1;
}
