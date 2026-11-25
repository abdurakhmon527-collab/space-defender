#pragma once

#include <filesystem>

namespace space_survivor {

class SaveSystem {
public:
  explicit SaveSystem(std::filesystem::path filePath = "saves/highscore.txt");

  [[nodiscard]] int loadHighScore() const;
  [[nodiscard]] bool saveHighScore(int score) const;

private:
  std::filesystem::path filePath_;
};

}  // namespace space_survivor
