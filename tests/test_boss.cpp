#include "Boss.hpp"

#include <iostream>
#include <limits>
#include <vector>

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
  Boss boss;
  check(!boss.isActive(), "boss starts inactive");
  boss.spawn({640.0F, 110.0F});
  check(boss.isActive() && boss.getHealth() == boss.getMaxHealth(),
     "spawn resets boss state");
  std::vector<Projectile> hostile;
  for (int idx = 0; idx < 24; ++idx) {
    boss.update(0.05F, {640.0F, 600.0F}, hostile);
  }
  check(hostile.size() == 3U, "boss fires a three-projectile spread");
  std::vector<Projectile> capped;
  for (int idx = 0; idx < 94; ++idx) {
    capped.emplace_back(sf::Vector2f{0.0F, 0.0F}, sf::Vector2f{0.0F, 1.0F},
                        1.0F, 1, true, sf::Color(255, 0, 0));
  }
  for (int idx = 0; idx < 24; ++idx) {
    boss.update(0.05F, {640.0F, 600.0F}, capped);
  }
  check(capped.size() == 94U, "boss volley respects the hostile projectile cap");
  boss.takeDamage(1);
  check(boss.getHealth() == boss.getMaxHealth() - 1, "boss takes damage");
  boss.takeDamage(std::numeric_limits<int>::max());
  check(!boss.isActive() && boss.isDefeated() && boss.getHealth() == 0,
     "lethal damage cannot underflow boss health");
  boss.takeDamage(1);
  check(boss.getHealth() == 0, "defeated boss ignores later damage");
  return failures == 0 ? 0 : 1;
}
