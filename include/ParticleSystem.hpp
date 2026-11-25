#pragma once

#include <SFML/Graphics.hpp>

#include <cstddef>
#include <vector>

namespace space_survivor {

class ParticleSystem {
public:
  ParticleSystem();

  void emit(const sf::Vector2f& position, const sf::Color& color,
       int count = 18, float speed = 170.0F);
  void emitTrail(const sf::Vector2f& position, const sf::Color& color);
  void update(float deltaTime) noexcept;
  void draw(sf::RenderTarget& target) const;
  void clear() noexcept { particles_.clear(); }

  [[nodiscard]] std::size_t size() const noexcept { return particles_.size(); }
  [[nodiscard]] static constexpr std::size_t capacity() noexcept { return MaxParticles; }

private:
  struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float radius{2.0F};
    float life{0.5F};
    float maxLife{0.5F};
  };

  std::vector<Particle> particles_;
  static constexpr std::size_t MaxParticles = 1200U;
};

}  // namespace space_survivor
