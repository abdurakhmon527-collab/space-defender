#include "PowerUp.hpp"

#include <iostream>

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
  PowerUp pickup(PowerUpType::Health, {20.0F, 20.0F});
  check(pickup.isAlive() && pickup.getType() == PowerUpType::Health,
     "pickup starts active with its selected type");
  pickup.update(0.05F);
  check(pickup.getPosition().y > 20.0F, "pickup drifts down the screen");
  check(pickup.getAge() > 0.0F, "pickup lifetime advances");
  pickup.collect();
  check(!pickup.isAlive(), "collecting removes the pickup");

  PowerUp expired(PowerUpType::Shield, {50.0F, 50.0F});
  for (int idx = 0; idx < 240; ++idx) {
    expired.update(0.05F);
  }
  check(!expired.isAlive(), "pickup expires after twelve seconds");
  return failures == 0 ? 0 : 1;
}
