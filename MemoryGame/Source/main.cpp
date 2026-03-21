#include "Engine/Game.h"
#include "GameScene.h"

#include <memory>

int main()
{
    Engine::Game game("Memory Game", 600, 630);
    game.run(std::make_unique<GameScene>(game));
    return 0;
}
