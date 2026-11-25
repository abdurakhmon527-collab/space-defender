#pragma once

#include <SFML/Graphics/Color.hpp>

#include <cstddef>

// AUDIT-001: Public project types share one namespace instead of polluting the global scope.
namespace space_survivor {

namespace config {
inline constexpr unsigned int WindowWidth = 1280U;
inline constexpr unsigned int WindowHeight = 720U;
inline constexpr int StartingHealth = 3;
inline constexpr float PlayerInvulnerabilityDuration = 1.15F;
inline constexpr float PlayerMoveSpeed = 360.0F;
inline constexpr float PlayerCollisionRadius = 23.0F;
inline constexpr int BossScoreThreshold = 1000;
inline constexpr float BossTimeThreshold = 120.0F;
inline constexpr int BossDefeatBonus = 1500;
inline constexpr float WaveDuration = 30.0F;
// AUDIT-006: Pickup effects use documented fixed durations.
inline constexpr float RapidFireDuration = 10.0F;
inline constexpr float ShieldDuration = 8.0F;
inline constexpr float PowerUpLifetime = 12.0F;
inline constexpr std::size_t MaxEnemies = 32U;
inline constexpr std::size_t MaxAsteroids = 24U;
inline constexpr std::size_t MaxPowerUps = 10U;
inline constexpr std::size_t MaxHostileProjectiles = 96U;
inline constexpr float MaxFrameDelta = 0.05F;
inline constexpr float Pi = 3.14159265358979323846F;
}  // namespace config

enum class GameState {
  MainMenu,
  Playing,
  Paused,
  GameOver,
  Victory,
  Settings,
  Controls,
  HighScores
};

enum class EnemyType { Basic, Fast, Heavy };
enum class PowerUpType { Health, RapidFire, Shield };
enum class SoundType { Shoot, Explosion, Damage, PowerUp, Menu };

}  // namespace space_survivor
