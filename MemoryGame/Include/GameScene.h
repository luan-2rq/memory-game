#pragma once

#include "Engine/Scene.h"
#include "GameModel.h"

#include <SFML/Graphics.hpp>

namespace Engine { class Game; }

class GameScene : public Engine::Scene
{
public:
    explicit GameScene(Engine::Game& game, GameModelConfig cfg = GameModelConfig{});

    void handleEvent(const sf::Event& event) override;
    void update(float dt)                    override;
    void draw(sf::RenderWindow& window)      override;

    [[nodiscard]] const GameModel& model() const;

private:
    Engine::Game& game_;

    GameModel model_;

    sf::Font font_;
    bool     fontLoaded_{ false };

    sf::RectangleShape restartBtn_;
    sf::Text           restartTxt_;
    sf::Text           movesTxt_;
    sf::Text           titleTxt_;
    sf::Text           statusTxt_;
    sf::Text           levelTxt_;
};
