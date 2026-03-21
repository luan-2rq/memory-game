#pragma once

#include <SFML/Graphics.hpp>

#include <memory>
#include <string>

namespace Engine
{

class Scene;

class Game
{
public:
    Game(const std::string& title, unsigned int width, unsigned int height);

    void run(std::unique_ptr<Scene> startScene);

    bool step(float dt);

    void setScene(std::unique_ptr<Scene> scene);

    void quit();

    sf::RenderWindow& getWindow();

private:
    sf::RenderWindow        window_;
    std::unique_ptr<Scene>  currentScene_;
    std::unique_ptr<Scene>  pendingScene_;
    bool                    running_{ true };
};

}
