#include "ParticleSystem.hpp"

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
  ParticleSystem particles;
  particles.emit({10.0F, 10.0F}, sf::Color::Cyan, 5000, 100.0F);
  check(particles.size() == ParticleSystem::capacity(), "particle count is capped");
  particles.emitTrail({10.0F, 10.0F}, sf::Color::White);
  check(particles.size() == ParticleSystem::capacity(), "trail cannot exceed the cap");
  particles.update(0.05F);
  check(particles.size() <= ParticleSystem::capacity(), "updates preserve the cap");
  for (int idx = 0; idx < 20; ++idx) {
    particles.update(0.05F);
  }
  check(particles.size() == 0U, "expired particles are removed from the pool");
  particles.clear();
  check(particles.size() == 0U, "clear removes all particles");
  particles.emit({0.0F, 0.0F}, sf::Color::White, -10, 100.0F);
  check(particles.size() == 0U, "negative emission count is ignored");
  return failures == 0 ? 0 : 1;
}
