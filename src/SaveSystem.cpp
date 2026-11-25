#include "SaveSystem.hpp"

#include <fstream>
#include <limits>
#include <string>
#include <system_error>
#include <utility>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace space_survivor {

SaveSystem::SaveSystem(std::filesystem::path filePath)
  : filePath_(std::move(filePath)) {}

int SaveSystem::loadHighScore() const {
  std::ifstream input(filePath_);
  if (!input) {
    return 0;
  }

  // AUDIT-003: Accept one complete non-negative integer, not a valid prefix.
  long long value = 0;
  if (!(input >> value) || value < 0 ||
    value > static_cast<long long>(std::numeric_limits<int>::max())) {
    return 0;
  }

  input >> std::ws;
  if (!input.eof()) {
    return 0;
  }
  return static_cast<int>(value);
}

bool SaveSystem::saveHighScore(int score) const {
  std::error_code error;
  const auto parent = filePath_.parent_path();
  if (!parent.empty()) {
    std::filesystem::create_directories(parent, error);
    if (error) {
      return false;
    }
  }

  // AUDIT-004: Keep the previous record in place until the replacement is ready.
  auto temporaryPath = filePath_;
  temporaryPath += ".tmp";
  {
    std::ofstream output(temporaryPath, std::ios::out | std::ios::trunc);
    if (!output) {
      return false;
    }
    output << (score < 0 ? 0 : score) << '\n';
    output.flush();
    output.close();
    if (!output.good()) {
      std::filesystem::remove(temporaryPath, error);
      return false;
    }
  }

#ifdef _WIN32
  const DWORD flags = MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH;
  if (MoveFileExW(temporaryPath.c_str(), filePath_.c_str(), flags) == 0) {
    std::filesystem::remove(temporaryPath, error);
    return false;
  }
#else
  std::filesystem::rename(temporaryPath, filePath_, error);
  if (error) {
    std::filesystem::remove(temporaryPath, error);
    return false;
  }
#endif
  return true;
}

}  // namespace space_survivor
