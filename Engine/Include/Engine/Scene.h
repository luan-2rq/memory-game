#pragma once

#include <SFML/Graphics.hpp>

namespace Engine
{

class Scene
{
public:
    virtual ~Scene() = default;

    virtual void handleEvent(const sf::Event&) {}
    virtual void update(float) {}
    virtual void draw(sf::RenderWindow&) {}
};

}
