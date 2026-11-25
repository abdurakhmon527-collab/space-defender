#include "Enemy.hpp"

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
  Enemy basic(EnemyType::Basic, {100.0F, 100.0F});
  Enemy fast(EnemyType::Fast, {100.0F, 100.0F});
  Enemy heavy(EnemyType::Heavy, {100.0F, 100.0F});
  check(basic.getHealth() == 2 && basic.getScoreReward() == 100,
     "basic enemy uses its expected stats");
  check(fast.getHealth() == 1 && fast.getScoreReward() == 150,
     "fast enemy uses its expected stats");
  check(heavy.getHealth() == 6 && heavy.getDamage() == 2,
     "heavy enemy uses its expected stats");
  basic.update(0.05F, {100.0F, 300.0F});
  check(basic.getPosition().y > 100.0F, "enemy moves toward the player");
  heavy.takeDamage(2);
  check(heavy.getHealth() == 4 && heavy.isAlive(), "partial damage keeps enemy alive");
  heavy.takeDamage(std::numeric_limits<int>::max());
  check(heavy.getHealth() == 0 && !heavy.isAlive(), "lethal damage removes enemy");
  basic.takeDamage(-5);
  check(basic.getHealth() == 2, "negative damage is ignored");
  fast.update(0.05F, {100.0F, 1000.0F});
  for (int idx = 0; idx < 240; ++idx) {
    fast.update(0.05F, {100.0F, 1000.0F});
  }
  check(!fast.isAlive(), "enemy leaving the play area is retired");
  return failures == 0 ? 0 : 1;
}
