#include "Engine/Game.h"
#include "Engine/Scene.h"

#include <SFML/Graphics.hpp>

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace Engine
{

namespace
{

std::string getEnvOrEmpty(const char* name)
{
    const char* value = std::getenv(name);
    return value == nullptr ? std::string{} : std::string(value);
}

}

Game::Game(const std::string& title, unsigned int width, unsigned int height, Mode mode)
    : mode_(mode)
{
    if (mode_ == Mode::Headed)
    {
        window_ = std::make_unique<sf::RenderWindow>(
            sf::VideoMode(width, height),
            title,
            sf::Style::Titlebar | sf::Style::Close);
        window_->setFramerateLimit(60);

        frameCaptureDirectory_ = getEnvOrEmpty("MEMORYGAME_CAPTURE_FRAMES_DIR");
        if (!frameCaptureDirectory_.empty())
        {
            std::filesystem::create_directories(frameCaptureDirectory_);
        }
    }
}

void Game::run(std::unique_ptr<Scene> startScene)
{
    currentScene_ = std::move(startScene);

    sf::Clock clock;

    while (isRunning())
    {
        const float dt = clock.restart().asSeconds();

        if (window_)
        {
            sf::Event event{};
            while (window_->pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window_->close();

                if (currentScene_)
                    currentScene_->handleEvent(event);
            }
        }

        // Apply any scene transition requested during event handling
        if (pendingScene_)
            currentScene_ = std::move(pendingScene_);

        // --- Update ---
        if (currentScene_)
            currentScene_->update(dt);

        // Apply any scene transition requested during update
        if (pendingScene_)
            currentScene_ = std::move(pendingScene_);

        // --- Draw ---
        // The scene is responsible for clearing the window to its background colour
        if (window_)
        {
            if (currentScene_)
                currentScene_->draw(*window_);

            captureFrameIfRequested();
            window_->display();
        }
    }
}

void Game::setScene(std::unique_ptr<Scene> scene)
{
    pendingScene_ = std::move(scene);
}

bool Game::step(float dt)
{
    if (!isRunning())
        return false;

    if (pendingScene_)
        currentScene_ = std::move(pendingScene_);

    if (window_)
    {
        sf::Event event{};
        while (window_->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window_->close();
                running_ = false;
            }
            if (currentScene_)
                currentScene_->handleEvent(event);
        }
    }

    if (pendingScene_)
        currentScene_ = std::move(pendingScene_);

    if (currentScene_)
        currentScene_->update(dt);

    if (pendingScene_)
        currentScene_ = std::move(pendingScene_);

    if (window_)
    {
        if (currentScene_)
            currentScene_->draw(*window_);

        captureFrameIfRequested();
        window_->display();
    }

    return isRunning();
}

void Game::quit()
{
    running_ = false;
    if (window_)
        window_->close();
}

bool Game::isRunning() const
{
    return running_ && (!window_ || window_->isOpen());
}

bool Game::hasWindow() const
{
    return window_ != nullptr;
}

void Game::captureFrameIfRequested()
{
    if (!window_ || frameCaptureDirectory_.empty())
    {
        return;
    }

    sf::Texture texture;
    texture.create(window_->getSize().x, window_->getSize().y);
    texture.update(*window_);

    std::ostringstream name;
    name << frameCaptureDirectory_ << "/frame-"
         << std::setw(5) << std::setfill('0') << frameCaptureIndex_++
         << ".png";

    texture.copyToImage().saveToFile(name.str());
}

sf::RenderWindow& Game::getWindow()
{
    assert(window_ && "Engine::Game::getWindow() requires headed mode");
    return *window_;
}

}
