#pragma once

#include "E2EFramework/Driver.h"
#include "GameModel.h"

#include <memory>

namespace Engine { class Game; }
class GameScene;

namespace MemoryGameTests
{

class GameDriver : public E2EFramework::Driver
{
public:
    enum class Mode { Headless, Headed };

    explicit GameDriver(Mode mode = Mode::Headless,
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
    Mode mode_;

    std::unique_ptr<GameModel> headlessModel_;

    std::unique_ptr<Engine::Game> game_;
    GameScene* scenePtr_{ nullptr };
};

}  // namespace MemoryGameTests
