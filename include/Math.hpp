#pragma once

#include <SFML/System/Vector2.hpp>

#include "Types.hpp"

#include <algorithm>
#include <cmath>

namespace space_survivor::math {

[[nodiscard]] inline float lengthSquared(const sf::Vector2f& value) noexcept {
  return value.x * value.x + value.y * value.y;
}

[[nodiscard]] inline float length(const sf::Vector2f& value) noexcept {
  return std::sqrt(lengthSquared(value));
}

[[nodiscard]] inline sf::Vector2f normalized(const sf::Vector2f& value) noexcept {
  const float magnitude = length(value);
  if (!std::isfinite(magnitude) || magnitude <= 0.0001F) {
    return {0.0F, 0.0F};
  }
  return value / magnitude;
}

[[nodiscard]] inline float distanceSquared(const sf::Vector2f& first,
                                           const sf::Vector2f& second) noexcept {
  return lengthSquared(first - second);
}

[[nodiscard]] inline float distance(const sf::Vector2f& first,
                  const sf::Vector2f& second) noexcept {
  return std::sqrt(distanceSquared(first, second));
}

[[nodiscard]] inline float clamp(float value, float minimum, float maximum) noexcept {
  if (!std::isfinite(value)) {
    return minimum;
  }
  return std::clamp(value, minimum, maximum);
}

// AUDIT-012: Reject non-finite deltas before they reach gameplay timers.
[[nodiscard]] inline float safeDelta(float deltaTime) noexcept {
  return std::isfinite(deltaTime) ? std::clamp(deltaTime, 0.0F, config::MaxFrameDelta) : 0.0F;
}

}  // namespace space_survivor::math
