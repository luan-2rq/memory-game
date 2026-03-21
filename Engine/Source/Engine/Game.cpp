#include "Engine/Game.h"
#include "Engine/Scene.h"

#include <SFML/Graphics.hpp>

namespace Engine
{

Game::Game(const std::string& title, unsigned int width, unsigned int height)
    : window_(sf::VideoMode(width, height), title,
              sf::Style::Titlebar | sf::Style::Close)
{
    window_.setFramerateLimit(60);
}

void Game::run(std::unique_ptr<Scene> startScene)
{
    currentScene_ = std::move(startScene);

    sf::Clock clock;

    while (window_.isOpen() && running_)
    {
        const float dt = clock.restart().asSeconds();

        // --- Events ---
        sf::Event event{};
        while (window_.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window_.close();

            if (currentScene_)
                currentScene_->handleEvent(event);
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
        if (currentScene_)
            currentScene_->draw(window_);

        window_.display();
    }
}

void Game::setScene(std::unique_ptr<Scene> scene)
{
    pendingScene_ = std::move(scene);
}

bool Game::step(float dt)
{
    if (!window_.isOpen() || !running_)
        return false;

    if (pendingScene_)
        currentScene_ = std::move(pendingScene_);

    sf::Event event{};
    while (window_.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window_.close();
            running_ = false;
        }
        if (currentScene_)
            currentScene_->handleEvent(event);
    }

    if (pendingScene_)
        currentScene_ = std::move(pendingScene_);

    if (currentScene_)
        currentScene_->update(dt);

    if (pendingScene_)
        currentScene_ = std::move(pendingScene_);

    if (currentScene_)
        currentScene_->draw(window_);

    window_.display();
    return window_.isOpen() && running_;
}

void Game::quit()
{
    running_ = false;
    window_.close();
}

sf::RenderWindow& Game::getWindow()
{
    return window_;
}

}
