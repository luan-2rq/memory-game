#include "Engine/Game.h"
#include "GameConstants.h"
#include "GameScene.h"

#include <memory>

int main()
{
    Engine::Game game("Memory Game", MemoryGame::kWindowWidth, MemoryGame::kWindowHeight);
    game.run(std::make_unique<GameScene>(game));
    return 0;
}
