#include "Asteroid.hpp"
#include "Collision.hpp"

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
  using space_survivor::Collision;
  check(Collision::circles({0.0F, 0.0F}, 5.0F, {10.0F, 0.0F}, 5.0F),
     "touching circles collide");
  check(!Collision::circles({0.0F, 0.0F}, 4.0F, {10.0F, 0.0F}, 5.0F),
     "separated circles do not collide");
  check(Collision::circles({0.0F, 0.0F}, -5.0F, {0.0F, 0.0F}, 0.0F),
     "negative radius is treated as zero");
  check(Collision::circleRectangle({5.0F, 5.0F}, 1.0F, {0.0F, 0.0F, 10.0F, 10.0F}),
     "circle centre inside rectangle collides");
  check(Collision::circleRectangle({12.0F, 5.0F}, 2.0F, {0.0F, 0.0F, 10.0F, 10.0F}),
     "circle tangent to rectangle collides");
  check(!Collision::circleRectangle({14.0F, 5.0F}, 2.0F, {0.0F, 0.0F, 10.0F, 10.0F}),
     "circle beyond rectangle does not collide");

  space_survivor::Asteroid asteroid({30.0F, 30.0F}, 35.0F, {0.0F, 15.0F}, 20.0F);
  asteroid.takeDamage(std::numeric_limits<int>::max());
  check(!asteroid.isAlive(), "large asteroid damage cannot underflow health");
  return failures == 0 ? 0 : 1;
}
