#pragma once

#include "Types.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace space_survivor::rules {
namespace {
constexpr float InitialEnemySpawnInterval = 1.35F;
constexpr float EnemySpawnWaveReduction = 0.11F;
constexpr float MinimumEnemySpawnInterval = 0.48F;
constexpr float InitialAsteroidSpawnInterval = 3.0F;
constexpr float AsteroidSpawnWaveReduction = 0.16F;
constexpr float MinimumAsteroidSpawnInterval = 1.0F;
}  // namespace

[[nodiscard]] inline int waveForTime(float seconds) noexcept {
  if (!std::isfinite(seconds) || seconds <= 0.0F) {
    return 1;
  }

  // AUDIT-012: Convert only after clamping in double precision.
  const double completedWaves = std::floor(
    static_cast<double>(seconds) / static_cast<double>(config::WaveDuration));
  const double maxCompleted =
    static_cast<double>(std::numeric_limits<int>::max() - 1);
  return 1 + static_cast<int>(std::min(completedWaves, maxCompleted));
}

[[nodiscard]] inline float enemySpawnInterval(int wave) noexcept {
  const int safeWave = std::max(1, wave);
  const float reduction = static_cast<float>(safeWave - 1) * EnemySpawnWaveReduction;
  return std::max(MinimumEnemySpawnInterval, InitialEnemySpawnInterval - reduction);
}

[[nodiscard]] inline float asteroidSpawnInterval(int wave) noexcept {
  const int safeWave = std::max(1, wave);
  const float reduction = static_cast<float>(safeWave - 1) * AsteroidSpawnWaveReduction;
  return std::max(MinimumAsteroidSpawnInterval, InitialAsteroidSpawnInterval - reduction);
}

[[nodiscard]] inline int addScore(int currentScore, int points) noexcept {
  const int score = std::max(0, currentScore);
  if (points <= 0) {
    return score;
  }
  const int maximum = std::numeric_limits<int>::max();
  return points > maximum - score ? maximum : score + points;
}

[[nodiscard]] inline bool shouldSpawnBoss(int score, float survivalSeconds) noexcept {
  // AUDIT-025: Either progression route must start the boss encounter.
  return score >= config::BossScoreThreshold ||
         (std::isfinite(survivalSeconds) && survivalSeconds >= config::BossTimeThreshold);
}

}  // namespace space_survivor::rules
