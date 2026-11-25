#include "Game.hpp"

#include "Collision.hpp"
#include "GameRules.hpp"
#include "Math.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

namespace space_survivor {
namespace {
constexpr unsigned int TargetFps = 60U;
constexpr std::size_t StarCount = 180U;
constexpr std::size_t MaxPlayerProjectiles = 100U;
constexpr float PlayerStartOffset = 92.0F;
constexpr float BackgroundLineSpacing = 120.0F;
constexpr float MenuVolumeStep = 10.0F;
constexpr float StarMinSpeed = 18.0F;
constexpr float StarMaxSpeed = 105.0F;
constexpr float StarMinRadius = 0.7F;
constexpr float StarMaxRadius = 2.1F;
constexpr float StarPhaseSpeed = 2.0F;
constexpr float StarResetOffset = -4.0F;
constexpr float BossSpawnY = 110.0F;
constexpr float EnemySpawnMargin = 36.0F;
constexpr float EnemySpawnY = -35.0F;
constexpr int EnemyRollMin = 1;
constexpr int EnemyRollMax = 100;
constexpr int HeavyEnemyMinWave = 3;
constexpr int HeavyEnemyRollThreshold = 83;
constexpr int FastEnemyRollThreshold = 62;
constexpr float AsteroidSpawnMargin = 30.0F;
constexpr float AsteroidMinRadius = 17.0F;
constexpr float AsteroidMaxRadius = 39.0F;
constexpr float AsteroidMinHorizontalSpeed = -58.0F;
constexpr float AsteroidMaxHorizontalSpeed = 58.0F;
constexpr float AsteroidMinFallSpeed = 95.0F;
constexpr float AsteroidMaxFallSpeed = 190.0F;
constexpr float AsteroidMinRotationSpeed = -85.0F;
constexpr float AsteroidMaxRotationSpeed = 85.0F;
constexpr int PowerUpRollMin = 0;
constexpr int PowerUpRollMax = 99;
constexpr int RapidFireRollThreshold = 38;
constexpr int ShieldRollThreshold = 72;
constexpr float PickupDropRollMin = 0.0F;
constexpr float PickupDropRollMax = 100.0F;
constexpr float PowerUpDropChance = 15.0F;
constexpr float PlayerAsteroidCollisionScale = 0.82F;
constexpr int AsteroidScoreReward = 25;
// AUDIT-016: Keep gameplay tuning in named constants rather than inline expressions.
}  // namespace

Game::Game()
  : window_(sf::VideoMode(config::WindowWidth, config::WindowHeight),
       "SPACE SURVIVOR - NEON EDITION", sf::Style::Titlebar | sf::Style::Close),
   randomEngine_(std::random_device{}()) {
  window_.setVerticalSyncEnabled(true);
  window_.setFramerateLimit(TargetFps);
  window_.setKeyRepeatEnabled(false);
  playerProjectiles_.reserve(MaxPlayerProjectiles);
  hostileProjectiles_.reserve(config::MaxHostileProjectiles);
  enemies_.reserve(config::MaxEnemies);
  asteroids_.reserve(config::MaxAsteroids);
  powerUps_.reserve(config::MaxPowerUps);
  loadResources();
  highScore_ = saveSystem_.loadHighScore();
  initializeStars();
  configureMenu();
}

void Game::run() {
  sf::Clock clock;
  while (window_.isOpen()) {
    const float deltaTime = math::safeDelta(clock.restart().asSeconds());
    processEvents();
    // AUDIT-027: Do not update or draw after the close event destroys the window.
    if (!window_.isOpen()) {
      break;
    }
    update(deltaTime);
    draw();
  }
}

void Game::loadResources() {
  const std::vector<std::string> fontCandidates{
    "assets/fonts/Orbitron-Regular.ttf",
    "C:/Windows/Fonts/arial.ttf",
    "C:/Windows/Fonts/consola.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf"
  };
  for (const auto& path : fontCandidates) {
    if (font_.loadFromFile(path)) {
      hasFont_ = true;
      break;
    }
  }
  if (!hasFont_) {
    std::cerr << "No font found. The game remains playable; text labels are hidden.\n";
  }
  audio_.load("assets/audio");
}

void Game::initializeStars() {
  stars_.clear();
  stars_.reserve(StarCount);
  for (std::size_t idx = 0; idx < StarCount; ++idx) {
    stars_.push_back({
      {randomFloat(0.0F, static_cast<float>(config::WindowWidth)),
             randomFloat(0.0F, static_cast<float>(config::WindowHeight))},
      randomFloat(StarMinSpeed, StarMaxSpeed),
      randomFloat(StarMinRadius, StarMaxRadius),
      randomFloat(0.0F, 2.0F * config::Pi)
    });
  }
}

void Game::resetGame() {
  score_ = 0;
  survivalTime_ = 0.0F;
  wave_ = 1;
  enemySpawnTimer_ = 0.0F;
  asteroidSpawnTimer_ = 0.0F;
  bossSpawned_ = false;
  moveUpPressed_ = false;
  moveDownPressed_ = false;
  moveLeftPressed_ = false;
  moveRightPressed_ = false;
  firePressed_ = false;
  player_.reset({static_cast<float>(config::WindowWidth) / 2.0F,
                   static_cast<float>(config::WindowHeight) - PlayerStartOffset});
  boss_ = Boss{};
  playerProjectiles_.clear();
  hostileProjectiles_.clear();
  enemies_.clear();
  asteroids_.clear();
  powerUps_.clear();
  particles_.clear();
  state_ = GameState::Playing;
  audio_.play(SoundType::Menu);
}

void Game::processEvents() {
  sf::Event event{};
  while (window_.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window_.close();
      continue;
    }
    if (event.type == sf::Event::LostFocus) {
      moveUpPressed_ = false;
      moveDownPressed_ = false;
      moveLeftPressed_ = false;
      moveRightPressed_ = false;
      firePressed_ = false;
      continue;
    }
    if (event.type == sf::Event::KeyPressed ||
        event.type == sf::Event::KeyReleased) {
      const bool pressed = event.type == sf::Event::KeyPressed;
      if (event.key.code == sf::Keyboard::W ||
          event.key.code == sf::Keyboard::Up) {
        moveUpPressed_ = pressed;
      }
      if (event.key.code == sf::Keyboard::S ||
          event.key.code == sf::Keyboard::Down) {
        moveDownPressed_ = pressed;
      }
      if (event.key.code == sf::Keyboard::A ||
          event.key.code == sf::Keyboard::Left) {
        moveLeftPressed_ = pressed;
      }
      if (event.key.code == sf::Keyboard::D ||
          event.key.code == sf::Keyboard::Right) {
        moveRightPressed_ = pressed;
      }
      if (event.key.code == sf::Keyboard::Space) {
        firePressed_ = pressed;
      }
    }

    if (state_ == GameState::Playing) {
      if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape) {
        state_ = GameState::Paused;
        configureMenu();
      }
      continue;
    }

    if (state_ == GameState::Paused && event.type == sf::Event::KeyPressed &&
      event.key.code == sf::Keyboard::Escape) {
      state_ = GameState::Playing;
      continue;
    }
    if (state_ == GameState::Settings && event.type == sf::Event::KeyPressed &&
      event.key.code == sf::Keyboard::M) {
      audio_.toggleMute();
      configureMenu();
      continue;
    }

    const sf::Vector2f mousePosition =
      window_.mapPixelToCoords(sf::Mouse::getPosition(window_));
    const int action = menu_.handleEvent(event, mousePosition);
    if (action >= 0) {
      audio_.play(SoundType::Menu);
      handleMenuAction(action);
    }
  }
}

void Game::configureMenu() {
  switch (state_) {
    case GameState::MainMenu:
      menu_.setContent("SPACE SURVIVOR",
                             "PILOT YOUR SHIP  |  DESTROY ENEMIES  |  SURVIVE THE VOID",
                             {"NEW GAME", "HIGH SCORES", "SETTINGS", "CONTROLS", "EXIT"});
      break;
    case GameState::Paused:
      menu_.setContent("PAUSED", "The simulation is stopped",
                             {"RESUME", "RESTART", "MAIN MENU"});
      break;
    case GameState::GameOver:
      menu_.setContent("SIGNAL LOST", "FINAL SCORE: " + std::to_string(score_),
                             {"RESTART", "MAIN MENU", "EXIT"});
      break;
    case GameState::Victory:
      menu_.setContent("SECTOR CLEARED",
                             "BOSS DEFEATED  |  FINAL SCORE: " + std::to_string(score_),
                             {"RESTART", "MAIN MENU", "EXIT"});
      break;
    case GameState::Settings: {
      const std::string volumeText = "AUDIO VOLUME: " +
        std::to_string(static_cast<int>(audio_.getVolume())) +
        "%  |  M TOGGLES MUTE";
      menu_.setContent("SETTINGS", volumeText,
                             {"VOLUME DOWN", "VOLUME UP", "MUTE / UNMUTE", "BACK"});
      break;
    }
    case GameState::Controls:
      menu_.setContent("FLIGHT CONTROLS",
                             "WASD OR ARROWS: MOVE  |  SPACE: FIRE  |  ESC: PAUSE",
                             {"BACK"});
      break;
    case GameState::HighScores:
      menu_.setContent("HIGH SCORES", "BEST RUN: " + std::to_string(highScore_),
                             {"BACK"});
      break;
    case GameState::Playing:
      break;
  }
}

void Game::handleMenuAction(int action) {
  switch (state_) {
    case GameState::MainMenu:
      handleMainMenuAction(action);
      break;
    case GameState::Paused:
      handlePauseAction(action);
      break;
    case GameState::GameOver:
    case GameState::Victory:
      handleEndStateAction(action);
      break;
    case GameState::Settings:
      handleSettingsAction(action);
      break;
    case GameState::Controls:
    case GameState::HighScores:
      returnToMainMenu();
      break;
    case GameState::Playing:
      break;
  }
}

void Game::handleMainMenuAction(int action) {
  switch (action) {
    case 0:
      resetGame();
      break;
    case 1:
      state_ = GameState::HighScores;
      configureMenu();
      break;
    case 2:
      state_ = GameState::Settings;
      configureMenu();
      break;
    case 3:
      state_ = GameState::Controls;
      configureMenu();
      break;
    case 4:
      window_.close();
      break;
    default:
      break;
  }
}

void Game::handlePauseAction(int action) {
  switch (action) {
    case 0:
      state_ = GameState::Playing;
      break;
    case 1:
      resetGame();
      break;
    case 2:
      returnToMainMenu();
      break;
    default:
      break;
  }
}

void Game::handleEndStateAction(int action) {
  if (action == 0) {
    resetGame();
  } else if (action == 1) {
    returnToMainMenu();
  } else if (action == 2) {
    window_.close();
  }
}

void Game::handleSettingsAction(int action) {
  if (action == 0) {
    audio_.setVolume(audio_.getVolume() - MenuVolumeStep);
  } else if (action == 1) {
    audio_.setVolume(audio_.getVolume() + MenuVolumeStep);
  } else if (action == 2) {
    audio_.toggleMute();
  } else if (action == 3) {
    returnToMainMenu();
  }
  configureMenu();
}

void Game::returnToMainMenu() {
  state_ = GameState::MainMenu;
  configureMenu();
}

// AUDIT-015: Keep frame coordination small; entity and collision work lives in helpers.
void Game::update(float deltaTime) {
  const float dt = math::safeDelta(deltaTime);
  animationTime_ += dt;
  updateStars(dt);
  if (state_ != GameState::Paused) {
    particles_.update(dt);
  }
  if (state_ != GameState::Playing) {
    return;
  }

  survivalTime_ += dt;
  wave_ = rules::waveForTime(survivalTime_);
  updatePlayer(dt);
  updateEntities(dt);
  handleCollisions();
  removeInactiveObjects();

  // AUDIT-023: Persist the high score as soon as the active run exceeds it.
  if (score_ > highScore_) {
    highScore_ = score_;
    (void)saveSystem_.saveHighScore(highScore_);
  }
  if (player_.getHealth() <= 0) {
    finishGame(false);
  } else if (boss_.isDefeated()) {
    addScore(config::BossDefeatBonus);
    finishGame(true);
  }
}

void Game::updatePlayer(float deltaTime) {
  player_.update(deltaTime, readMovementInput());
  // AUDIT-024: Continuous fire has a hard cap while projectiles are still in flight.
  if ((firePressed_ || sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) &&
    playerProjectiles_.size() + 2U <= MaxPlayerProjectiles) {
    const std::size_t oldSize = playerProjectiles_.size();
    player_.shoot(playerProjectiles_, player_.hasRapidFire());
    if (playerProjectiles_.size() > oldSize) {
      audio_.play(SoundType::Shoot);
    }
  }
}

void Game::updateEntities(float deltaTime) {
  enemySpawnTimer_ += deltaTime;
  if (enemySpawnTimer_ >= rules::enemySpawnInterval(wave_)) {
    spawnEnemy();
    enemySpawnTimer_ = 0.0F;
  }

  asteroidSpawnTimer_ += deltaTime;
  if (asteroidSpawnTimer_ >= rules::asteroidSpawnInterval(wave_)) {
    spawnAsteroid();
    asteroidSpawnTimer_ = 0.0F;
  }

  spawnBossIfNeeded();
  for (auto& projectile : playerProjectiles_) {
    projectile.update(deltaTime);
    particles_.emitTrail(projectile.getPosition(), sf::Color(60, 220, 255));
  }
  for (auto& projectile : hostileProjectiles_) {
    projectile.update(deltaTime);
  }
  for (auto& enemy : enemies_) {
    enemy.update(deltaTime, player_.getPosition());
  }
  for (auto& asteroid : asteroids_) {
    asteroid.update(deltaTime);
  }
  for (auto& powerUp : powerUps_) {
    powerUp.update(deltaTime);
  }
  boss_.update(deltaTime, player_.getPosition(), hostileProjectiles_);
}

void Game::spawnBossIfNeeded() {
  if (bossSpawned_ || !rules::shouldSpawnBoss(score_, survivalTime_)) {
    return;
  }
  boss_.spawn({static_cast<float>(config::WindowWidth) / 2.0F, BossSpawnY});
  bossSpawned_ = true;
  particles_.emit(boss_.getPosition(), sf::Color(255, 65, 165), 75, 220.0F);
}

void Game::updateStars(float deltaTime) {
  for (auto& star : stars_) {
    star.position.y += star.speed * deltaTime;
    star.phase += deltaTime * StarPhaseSpeed;
    if (star.position.y > static_cast<float>(config::WindowHeight)) {
      star.position.y = StarResetOffset;
      star.position.x = randomFloat(0.0F, static_cast<float>(config::WindowWidth));
    }
  }
}

void Game::spawnEnemy() {
  if (enemies_.size() >= config::MaxEnemies) {
    return;
  }
  const float x = randomFloat(
    EnemySpawnMargin, static_cast<float>(config::WindowWidth) - EnemySpawnMargin);
  const int roll = randomInt(EnemyRollMin, EnemyRollMax);
  EnemyType type = EnemyType::Basic;
  if (wave_ >= HeavyEnemyMinWave && roll > HeavyEnemyRollThreshold) {
    type = EnemyType::Heavy;
  } else if (roll > FastEnemyRollThreshold) {
    type = EnemyType::Fast;
  }
  enemies_.emplace_back(type, sf::Vector2f{x, EnemySpawnY});
}

void Game::spawnAsteroid() {
  if (asteroids_.size() >= config::MaxAsteroids) {
    return;
  }
  const float radius = randomFloat(AsteroidMinRadius, AsteroidMaxRadius);
  const float x = randomFloat(
    AsteroidSpawnMargin, static_cast<float>(config::WindowWidth) - AsteroidSpawnMargin);
  const sf::Vector2f velocity{
    randomFloat(AsteroidMinHorizontalSpeed, AsteroidMaxHorizontalSpeed),
    randomFloat(AsteroidMinFallSpeed, AsteroidMaxFallSpeed)
  };
  asteroids_.emplace_back(sf::Vector2f{x, -radius}, radius, velocity,
              randomFloat(AsteroidMinRotationSpeed, AsteroidMaxRotationSpeed));
}

void Game::spawnPowerUp(const sf::Vector2f& position) {
  if (powerUps_.size() >= config::MaxPowerUps) {
    return;
  }
  const int roll = randomInt(PowerUpRollMin, PowerUpRollMax);
  PowerUpType type = PowerUpType::Health;
  if (roll < RapidFireRollThreshold) {
    type = PowerUpType::RapidFire;
  } else if (roll < ShieldRollThreshold) {
    type = PowerUpType::Shield;
  }
  powerUps_.emplace_back(type, position);
}

void Game::handleCollisions() {
  handlePlayerProjectileCollisions();
  handleContactCollisions();
  handlePowerUpCollisions();
}

void Game::handlePlayerProjectileCollisions() {
  for (auto& projectile : playerProjectiles_) {
    if (!projectile.isAlive()) {
      continue;
    }
    for (auto& enemy : enemies_) {
      if (!enemy.isAlive() ||
        !Collision::circles(projectile.getPosition(), projectile.getRadius(),
                  enemy.getPosition(), enemy.getRadius())) {
        continue;
      }
      projectile.destroy();
      enemy.takeDamage(projectile.getDamage());
      if (!enemy.isAlive()) {
        addScore(enemy.getScoreReward());
        particles_.emit(enemy.getPosition(), sf::Color(255, 75, 140), 24, 185.0F);
        audio_.play(SoundType::Explosion);
        if (randomFloat(PickupDropRollMin, PickupDropRollMax) < PowerUpDropChance) {
          spawnPowerUp(enemy.getPosition());
        }
      }
      break;
    }
    if (!projectile.isAlive()) {
      continue;
    }
    hitAsteroids(projectile);
    if (projectile.isAlive() && boss_.isActive() &&
      Collision::circles(projectile.getPosition(), projectile.getRadius(),
                               boss_.getPosition(), boss_.getRadius())) {
      projectile.destroy();
      boss_.takeDamage(projectile.getDamage());
      particles_.emit(projectile.getPosition(), sf::Color(255, 95, 190), 5, 85.0F);
    }
  }
}

void Game::hitAsteroids(Projectile& projectile) {
  for (auto& asteroid : asteroids_) {
    if (!asteroid.isAlive() ||
      !Collision::circles(projectile.getPosition(), projectile.getRadius(),
                asteroid.getPosition(), asteroid.getRadius())) {
      continue;
    }
    projectile.destroy();
    asteroid.takeDamage(projectile.getDamage());
    if (!asteroid.isAlive()) {
      addScore(AsteroidScoreReward);
      particles_.emit(asteroid.getPosition(), sf::Color(160, 145, 205), 18, 135.0F);
    }
    break;
  }
}

void Game::handleContactCollisions() {
  for (auto& enemy : enemies_) {
    if (!enemy.isAlive() ||
      !Collision::circles(player_.getPosition(), player_.getRadius(),
                enemy.getPosition(), enemy.getRadius())) {
      continue;
    }
    if (player_.takeDamage(enemy.getDamage())) {
      audio_.play(SoundType::Damage);
      particles_.emit(player_.getPosition(), sf::Color(255, 65, 100), 25, 190.0F);
    }
    enemy.destroy();
    particles_.emit(enemy.getPosition(), sf::Color(255, 75, 130), 13, 120.0F);
  }

  for (auto& asteroid : asteroids_) {
    if (!asteroid.isAlive() ||
      !Collision::circles(player_.getPosition(), player_.getRadius(),
                asteroid.getPosition(), asteroid.getRadius() * PlayerAsteroidCollisionScale)) {
      continue;
    }
    if (player_.takeDamage()) {
      audio_.play(SoundType::Damage);
      particles_.emit(player_.getPosition(), sf::Color(255, 95, 115), 26, 200.0F);
    }
    asteroid.destroy();
    particles_.emit(asteroid.getPosition(), sf::Color(155, 145, 195), 16, 145.0F);
  }

  for (auto& projectile : hostileProjectiles_) {
    if (!projectile.isAlive() ||
      !Collision::circles(player_.getPosition(), player_.getRadius(),
                projectile.getPosition(), projectile.getRadius())) {
      continue;
    }
    projectile.destroy();
    if (player_.takeDamage(projectile.getDamage())) {
      audio_.play(SoundType::Damage);
      particles_.emit(player_.getPosition(), sf::Color(255, 55, 130), 20, 175.0F);
    }
  }
}

void Game::handlePowerUpCollisions() {
  for (auto& powerUp : powerUps_) {
    if (!powerUp.isAlive() ||
      !Collision::circles(player_.getPosition(), player_.getRadius(),
                powerUp.getPosition(), powerUp.getRadius())) {
      continue;
    }
    player_.collect(powerUp.getType());
    powerUp.collect();
    particles_.emit(powerUp.getPosition(), sf::Color(80, 240, 255), 20, 150.0F);
    audio_.play(SoundType::PowerUp);
  }
}

void Game::removeInactiveObjects() {
  const auto removeInactive = [](auto& collection) {
    collection.erase(std::remove_if(collection.begin(), collection.end(),
                    [](const auto& object) {
                      return !object.isAlive();
                    }),
             collection.end());
  };
  removeInactive(playerProjectiles_);
  removeInactive(hostileProjectiles_);
  removeInactive(enemies_);
  removeInactive(asteroids_);
  removeInactive(powerUps_);
}

// AUDIT-014: Saturate score instead of overflowing after an unusually long run.
void Game::addScore(int points) noexcept {
  score_ = rules::addScore(score_, points);
}

void Game::finishGame(bool victory) {
  highScore_ = std::max(highScore_, score_);
  (void)saveSystem_.saveHighScore(highScore_);
  state_ = victory ? GameState::Victory : GameState::GameOver;
  configureMenu();
  if (victory) {
    particles_.emit({640.0F, 110.0F}, sf::Color(90, 255, 210), 110, 260.0F);
  } else {
    particles_.emit(player_.getPosition(), sf::Color(255, 65, 110), 90, 260.0F);
  }
}

void Game::draw() {
  window_.clear(sf::Color(3, 5, 16));
  drawBackground();
  if (state_ == GameState::Playing || state_ == GameState::Paused ||
    state_ == GameState::GameOver || state_ == GameState::Victory) {
    drawGameplay();
  }
  if (state_ != GameState::Playing) {
    menu_.draw(window_, font_, hasFont_, animationTime_);
  }
  window_.display();
}

void Game::drawBackground() {
  for (const auto& star : stars_) {
    const float twinkle = 0.55F + 0.45F * std::sin(star.phase);
    sf::CircleShape dot(star.radius * (0.8F + twinkle * 0.2F));
    dot.setPosition(star.position);
    const auto alpha = static_cast<sf::Uint8>(80.0F + twinkle * 150.0F);
    dot.setFillColor(sf::Color(100, 175, 235, alpha));
    window_.draw(dot);
  }

  for (float y = BackgroundLineSpacing; y < static_cast<float>(config::WindowHeight);
         y += BackgroundLineSpacing) {
    sf::RectangleShape line({static_cast<float>(config::WindowWidth), 1.0F});
    line.setPosition(0.0F, y);
    line.setFillColor(sf::Color(25, 70, 115, 22));
    window_.draw(line);
  }
}

void Game::drawGameplay() {
  for (const auto& projectile : playerProjectiles_) {
    projectile.draw(window_);
  }
  for (const auto& projectile : hostileProjectiles_) {
    projectile.draw(window_);
  }
  for (const auto& asteroid : asteroids_) {
    asteroid.draw(window_);
  }
  for (const auto& enemy : enemies_) {
    enemy.draw(window_);
  }
  for (const auto& powerUp : powerUps_) {
    powerUp.draw(window_);
  }
  boss_.draw(window_);
  player_.draw(window_);
  particles_.draw(window_);
  hud_.draw(window_, font_, hasFont_, player_, score_, highScore_, survivalTime_, wave_,
       boss_.isActive(), boss_.getHealth(), boss_.getMaxHealth());
}

sf::Vector2f Game::readMovementInput() const noexcept {
  sf::Vector2f direction{0.0F, 0.0F};
  if (moveUpPressed_) {
    direction.y -= 1.0F;
  }
  if (moveDownPressed_) {
    direction.y += 1.0F;
  }
  if (moveLeftPressed_) {
    direction.x -= 1.0F;
  }
  if (moveRightPressed_) {
    direction.x += 1.0F;
  }
  return direction;
}

float Game::randomFloat(float minimum, float maximum) {
  std::uniform_real_distribution<float> distribution(minimum, maximum);
  return distribution(randomEngine_);
}

int Game::randomInt(int minimum, int maximum) {
  std::uniform_int_distribution<int> distribution(minimum, maximum);
  return distribution(randomEngine_);
}

}  // namespace space_survivor
