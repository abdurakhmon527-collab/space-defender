#include "GameRules.hpp"
#include "Math.hpp"

#include <cmath>
#include <iostream>
#include <limits>

namespace {
int failures = 0;
void check(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
    ++failures;
  }
}
}  // namespace

int main() {
  using namespace space_survivor;
  const sf::Vector2f unit = math::normalized({3.0F, 4.0F});
  check(std::abs(math::length(unit) - 1.0F) < 0.0001F, "normalization returns unit length");
  check(math::normalized({0.0F, 0.0F}) == sf::Vector2f{0.0F, 0.0F},
     "zero vector stays zero");
  check(math::distance({0.0F, 0.0F}, {3.0F, 4.0F}) == 5.0F, "distance uses Euclidean length");
  check(math::clamp(12.0F, 0.0F, 10.0F) == 10.0F, "clamp limits high values");
  check(math::safeDelta(1.0F) == config::MaxFrameDelta, "frame delta is capped");
  check(math::safeDelta(-1.0F) == 0.0F, "negative frame delta is ignored");
  check(rules::waveForTime(29.9F) == 1, "first wave lasts 30 seconds");
  check(rules::waveForTime(30.0F) == 2, "second wave starts at 30 seconds");
  check(rules::waveForTime(std::numeric_limits<float>::max()) ==
       std::numeric_limits<int>::max(),
     "wave calculation saturates instead of overflowing");
  check(rules::shouldSpawnBoss(config::BossScoreThreshold, 0.0F),
     "score threshold triggers boss");
  check(rules::shouldSpawnBoss(0, config::BossTimeThreshold),
     "time threshold triggers boss");
  check(!rules::shouldSpawnBoss(999, 119.9F), "boss stays inactive below both thresholds");
  check(rules::addScore(100, 50) == 150, "score increases by a positive reward");
  check(rules::addScore(100, -50) == 100, "negative score rewards are ignored");
  check(rules::addScore(std::numeric_limits<int>::max() - 5, 10) ==
          std::numeric_limits<int>::max(),
        "score rewards saturate at INT_MAX");
  return failures == 0 ? 0 : 1;
}
