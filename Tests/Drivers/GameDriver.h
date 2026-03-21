#pragma once

#include "E2EFramework/Driver.h"
#include "Engine/Game.h"
#include "GameModel.h"

#include <memory>

class GameScene;

namespace MemoryGameTests
{

class GameDriver : public E2EFramework::Driver
{
public:
    explicit GameDriver(Engine::Game::Mode mode = Engine::Game::Mode::Headless,
                        GameModelConfig cfg = GameModelConfig{});
    ~GameDriver() override;

    GameDriver(const GameDriver&) = delete;
    GameDriver& operator=(const GameDriver&) = delete;

    void clickCard(int index);
    void clickRestart();
    void settle();

    void advance(float dt = 1.f / 60.f) override;
    [[nodiscard]] bool isRunning() const override;

    [[nodiscard]] const GameModel& model() const;

private:
    std::unique_ptr<Engine::Game> game_;
    GameScene* scenePtr_{ nullptr };
};

}  // namespace MemoryGameTests
