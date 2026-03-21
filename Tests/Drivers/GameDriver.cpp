#include "Drivers/GameDriver.h"

#include "Engine/Game.h"
#include "GameConstants.h"
#include "GameScene.h"

#include <SFML/Graphics.hpp>

namespace MemoryGameTests
{

static constexpr float kRestartCenterX = 300.f;
static constexpr float kRestartCenterY = 597.f;

static sf::Event makeLeftClick(float x, float y)
{
    sf::Event e{};
    e.type = sf::Event::MouseButtonPressed;
    e.mouseButton.button = sf::Mouse::Left;
    e.mouseButton.x = static_cast<int>(x);
    e.mouseButton.y = static_cast<int>(y);
    return e;
}

GameDriver::GameDriver(Engine::Game::Mode mode, GameModelConfig cfg)
{
    game_ = std::make_unique<Engine::Game>(
        "Memory Game E2E",
        MemoryGame::kWindowWidth,
        MemoryGame::kWindowHeight,
        mode);
    auto scene = std::make_unique<GameScene>(*game_, cfg);
    scenePtr_ = scene.get();
    game_->setScene(std::move(scene));
    game_->step(0.f);
}

GameDriver::~GameDriver() = default;

void GameDriver::clickCard(int index)
{
    const sf::FloatRect b = scenePtr_->model().cards()[index].getBounds();
    scenePtr_->handleEvent(makeLeftClick(
        b.left + b.width * 0.5f,
        b.top + b.height * 0.5f));
}

void GameDriver::clickRestart()
{
    scenePtr_->handleEvent(makeLeftClick(kRestartCenterX, kRestartCenterY));
}

void GameDriver::advance(float dt)
{
    game_->step(dt);
}

void GameDriver::settle()
{
    advance(2.f);
    advance(2.f);
}

const GameModel& GameDriver::model() const
{
    return scenePtr_->model();
}

bool GameDriver::isRunning() const
{
    return game_->isRunning();
}

bool GameDriver::saveScreenshot(const std::string& outputPath)
{
    if (!game_ || !game_->hasWindow())
    {
        return false;
    }

    sf::RenderWindow& window = game_->getWindow();
    sf::Texture texture;
    if (!texture.create(window.getSize().x, window.getSize().y))
    {
        return false;
    }

    texture.update(window);
    return texture.copyToImage().saveToFile(outputPath);
}

}  // namespace MemoryGameTests
