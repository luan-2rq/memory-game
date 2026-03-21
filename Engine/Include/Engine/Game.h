#pragma once

#include <SFML/Graphics.hpp>

#include <memory>
#include <string_view>
#include <string>

namespace Engine
{

class Scene;

class Game
{
public:
    enum class Mode
    {
        Headed,
        Headless
    };

    Game(const std::string& title,
         unsigned int width,
         unsigned int height,
         Mode mode = Mode::Headed);

    void run(std::unique_ptr<Scene> startScene);

    bool step(float dt);

    void setScene(std::unique_ptr<Scene> scene);

    void quit();

    [[nodiscard]] bool isRunning() const;
    [[nodiscard]] bool hasWindow() const;

    sf::RenderWindow& getWindow();

private:
    void captureFrameIfRequested();

    Mode                    mode_;
    std::unique_ptr<sf::RenderWindow> window_;
    std::unique_ptr<Scene>  currentScene_;
    std::unique_ptr<Scene>  pendingScene_;
    bool                    running_{ true };
    std::string             frameCaptureDirectory_;
    unsigned int            frameCaptureIndex_{ 0 };
};

}
