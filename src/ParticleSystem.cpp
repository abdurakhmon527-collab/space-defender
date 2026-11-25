#include "ParticleSystem.hpp"

#include "Math.hpp"

#include <algorithm>
#include <cmath>
#include <random>

namespace space_survivor {
namespace {
std::mt19937& particleRandom() {
  static std::mt19937 engine{std::random_device{}()};
  return engine;
}

float randomRange(float low, float high) {
  std::uniform_real_distribution<float> distribution(low, high);
  return distribution(particleRandom());
}
}  // namespace

// AUDIT-009: Reserve the fixed pool once; emitting particles then avoids vector growth.
ParticleSystem::ParticleSystem() {
  particles_.reserve(MaxParticles);
}

void ParticleSystem::emit(const sf::Vector2f& position, const sf::Color& color,
             int count, float speed) {
  if (count <= 0 || !std::isfinite(speed) || speed <= 0.0F) {
    return;
  }

  const std::size_t available = MaxParticles - particles_.size();
  const std::size_t requested = static_cast<std::size_t>(count);
  const std::size_t toEmit = std::min(available, requested);
  for (std::size_t idx = 0; idx < toEmit; ++idx) {
    const float angle = randomRange(0.0F, 2.0F * config::Pi);
    const float velocity = randomRange(speed * 0.25F, speed);
    Particle particle;
    particle.position = position;
    particle.velocity = {std::cos(angle) * velocity, std::sin(angle) * velocity};
    particle.color = color;
    particle.radius = randomRange(1.4F, 3.8F);
    particle.life = randomRange(0.28F, 0.85F);
    particle.maxLife = particle.life;
    particles_.push_back(particle);
  }
}

void ParticleSystem::emitTrail(const sf::Vector2f& position, const sf::Color& color) {
  if (particles_.size() >= MaxParticles) {
    return;
  }

  Particle particle;
  particle.position = position;
  particle.velocity = {randomRange(-8.0F, 8.0F), randomRange(28.0F, 65.0F)};
  particle.color = color;
  particle.radius = randomRange(1.0F, 2.2F);
  particle.life = 0.22F;
  particle.maxLife = particle.life;
  particles_.push_back(particle);
}

void ParticleSystem::update(float deltaTime) noexcept {
  const float dt = math::safeDelta(deltaTime);
  for (auto& particle : particles_) {
    particle.position += particle.velocity * dt;
    particle.velocity *= std::max(0.0F, 1.0F - dt * 1.7F);
    particle.life -= dt;
  }

  particles_.erase(std::remove_if(particles_.begin(), particles_.end(),
                                   [](const Particle& particle) {
                                       return particle.life <= 0.0F;
                                   }),
                     particles_.end());
}

void ParticleSystem::draw(sf::RenderTarget& target) const {
  for (const auto& particle : particles_) {
    const float ratio = math::clamp(particle.life / particle.maxLife, 0.0F, 1.0F);
    sf::CircleShape shape(particle.radius * (0.45F + ratio * 0.55F));
    shape.setOrigin(shape.getRadius(), shape.getRadius());
    shape.setPosition(particle.position);
    sf::Color color = particle.color;
    color.a = static_cast<sf::Uint8>(220.0F * ratio);
    shape.setFillColor(color);
    target.draw(shape);
  }
}

}  // namespace space_survivor
