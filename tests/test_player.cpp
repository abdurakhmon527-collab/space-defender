#include "Player.hpp"

#include <iostream>
#include <vector>

namespace {
int failures = 0;

void check(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
    ++failures;
  }
}

void testMovementAndBounds() {
  using namespace space_survivor;
  Player player;
  check(player.getHealth() == 3, "player starts with three health points");
  player.reset({640.0F, 590.0F});
  player.update(0.5F, {1.0F, 0.0F});
  check(player.getPosition().x > 640.0F, "player moves from supplied input");
  for (int idx = 0; idx < 120; ++idx) {
    player.update(0.05F, {1.0F, 0.0F});
  }
  check(player.getPosition().x <= 1249.0F, "player stays inside the right boundary");
}

void testShootingCooldown() {
  using namespace space_survivor;
  Player player;
  std::vector<Projectile> shots;
  player.shoot(shots, false);
  check(shots.size() == 2U, "one firing action creates two lasers");
  player.shoot(shots, false);
  check(shots.size() == 2U, "normal shot cooldown prevents immediate repeat");
  for (int idx = 0; idx < 6; ++idx) {
    player.update(0.05F, {0.0F, 0.0F});
  }
  player.shoot(shots, false);
  check(shots.size() == 4U, "cooldown permits the next shot");
}

void testPowerUpTimers() {
  using namespace space_survivor;
  Player player;
  player.collect(PowerUpType::RapidFire);
  check(player.hasRapidFire(), "rapid fire activates on collection");
  check(player.getRapidFireTime() == config::RapidFireDuration,
        "rapid fire lasts ten seconds");
  player.update(0.05F, {0.0F, 0.0F});
  player.collect(PowerUpType::RapidFire);
  check(player.getRapidFireTime() == config::RapidFireDuration,
        "collecting rapid fire refreshes rather than stacks its timer");
  for (int idx = 0; idx < 205; ++idx) {
    player.update(0.05F, {0.0F, 0.0F});
  }
  check(!player.hasRapidFire(), "rapid fire expires after its ten-second window");
  player.collect(PowerUpType::Shield);
  check(player.getShieldTime() == config::ShieldDuration, "shield lasts eight seconds");
  check(!player.takeDamage(), "shield blocks incoming damage");
  for (int idx = 0; idx < 165; ++idx) {
    player.update(0.05F, {0.0F, 0.0F});
  }
  check(!player.hasShield(), "shield expires after its eight-second window");
}

void testDamageAndInvulnerability() {
  using namespace space_survivor;
  Player player;
  player.reset({640.0F, 590.0F});
  check(player.takeDamage(2), "damage reduces health");
  check(player.getHealth() == 1, "damage amount is applied");
  check(!player.takeDamage(), "invulnerability window blocks repeated contact");
  player.update(0.05F, {0.0F, 0.0F});
  player.reset({640.0F, 590.0F});
  check(player.takeDamage(100), "large damage is accepted without integer underflow");
  check(player.getHealth() == 0, "health clamps at zero");
}
}  // namespace

int main() {
  // AUDIT-028: Keep each test scenario short enough to isolate a failed behavior.
  testMovementAndBounds();
  testShootingCooldown();
  testPowerUpTimers();
  testDamageAndInvulnerability();
  return failures == 0 ? 0 : 1;
}
