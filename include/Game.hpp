#pragma once

#include "Asteroid.hpp"
#include "AudioManager.hpp"
#include "Boss.hpp"
#include "Enemy.hpp"
#include "HUD.hpp"
#include "Menu.hpp"
#include "ParticleSystem.hpp"
#include "Player.hpp"
#include "PowerUp.hpp"
#include "SaveSystem.hpp"
#include "Types.hpp"

#include <SFML/Graphics.hpp>

#include <random>
#include <vector>

namespace space_survivor {

class Game {
public:
  Game();
  void run();

private:
  struct Star {
    sf::Vector2f position;
    float speed;
    float radius;
    float phase;
  };

  sf::RenderWindow window_;
  sf::Font font_;
  bool hasFont_{false};
  GameState state_{GameState::MainMenu};
  Menu menu_;
  HUD hud_;
  Player player_;
  Boss boss_;
  ParticleSystem particles_;
  SaveSystem saveSystem_;
  AudioManager audio_;
  std::vector<Projectile> playerProjectiles_;
  std::vector<Projectile> hostileProjectiles_;
  std::vector<Enemy> enemies_;
  std::vector<Asteroid> asteroids_;
  std::vector<PowerUp> powerUps_;
  std::vector<Star> stars_;
  std::mt19937 randomEngine_;

  int score_{0};
  int highScore_{0};
  int wave_{1};
  float survivalTime_{0.0F};
  float enemySpawnTimer_{0.0F};
  float asteroidSpawnTimer_{0.0F};
  float animationTime_{0.0F};
  bool bossSpawned_{false};
  bool moveUpPressed_{false};
  bool moveDownPressed_{false};
  bool moveLeftPressed_{false};
  bool moveRightPressed_{false};
  bool firePressed_{false};

  void loadResources();
  void initializeStars();
  void resetGame();
  void processEvents();
  void handleMenuAction(int action);
  void handleMainMenuAction(int action);
  void handlePauseAction(int action);
  void handleEndStateAction(int action);
  void handleSettingsAction(int action);
  void returnToMainMenu();
  void configureMenu();
  void update(float deltaTime);
  void updateStars(float deltaTime);
  void updatePlayer(float deltaTime);
  void updateEntities(float deltaTime);
  void spawnBossIfNeeded();
  void spawnEnemy();
  void spawnAsteroid();
  void spawnPowerUp(const sf::Vector2f& position);
  void handleCollisions();
  void handlePlayerProjectileCollisions();
  void hitAsteroids(Projectile& projectile);
  void handleContactCollisions();
  void handlePowerUpCollisions();
  void removeInactiveObjects();
  void addScore(int points) noexcept;
  void finishGame(bool victory);
  void draw();
  void drawBackground();
  void drawGameplay();
  [[nodiscard]] sf::Vector2f readMovementInput() const noexcept;
  [[nodiscard]] float randomFloat(float minimum, float maximum);
  [[nodiscard]] int randomInt(int minimum, int maximum);
};

}  // namespace space_survivor
