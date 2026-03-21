#include "GameScene.h"
#include "Engine/Game.h"

#include <SFML/Graphics.hpp>

#include <sstream>

static constexpr float kWinWidth = 600.f;

static constexpr float kBtnY   = 578.f;
static constexpr float kBtnW   = 160.f;
static constexpr float kBtnH   = 38.f;
static constexpr float kBtnX   = (kWinWidth - kBtnW) / 2.f;

const GameModel& GameScene::model() const { return model_; }

GameScene::GameScene(Engine::Game& game, GameModelConfig cfg)
    : game_(game), model_(cfg)
{
    static constexpr const char* kFontCandidates[] = {
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        nullptr
    };

    for (const char* const* p = kFontCandidates; *p; ++p)
    {
        if (font_.loadFromFile(*p))
        {
            fontLoaded_ = true;
            break;
        }
    }

    restartBtn_.setSize({ kBtnW, kBtnH });
    restartBtn_.setPosition(kBtnX, kBtnY);
    restartBtn_.setFillColor(sf::Color(55, 65, 115));
    restartBtn_.setOutlineColor(sf::Color(130, 145, 210));
    restartBtn_.setOutlineThickness(2.f);

    if (fontLoaded_)
    {
        auto setup = [&](sf::Text& t, unsigned int size)
        {
            t.setFont(font_);
            t.setCharacterSize(size);
            t.setFillColor(sf::Color(220, 220, 220));
        };

        setup(restartTxt_, 20);
        restartTxt_.setString("Restart");

        setup(movesTxt_, 21);
        movesTxt_.setFillColor(sf::Color(190, 190, 190));
        movesTxt_.setPosition(12.f, 10.f);

        setup(levelTxt_, 21);
        levelTxt_.setFillColor(sf::Color(190, 190, 190));

        setup(titleTxt_, 24);
        titleTxt_.setString("Memory Game");
        titleTxt_.setFillColor(sf::Color(160, 210, 255));
        {
            const sf::FloatRect b = titleTxt_.getLocalBounds();
            titleTxt_.setOrigin(b.left + b.width / 2.f, b.top);
            titleTxt_.setPosition(kWinWidth / 2.f, 10.f);
        }

        setup(statusTxt_, 26);
        statusTxt_.setFillColor(sf::Color(255, 220, 70));
    }
}

void GameScene::handleEvent(const sf::Event& event)
{
    if (event.type != sf::Event::MouseButtonPressed) return;
    if (event.mouseButton.button != sf::Mouse::Left)  return;

    const sf::Vector2f pos(
        static_cast<float>(event.mouseButton.x),
        static_cast<float>(event.mouseButton.y));

    if (restartBtn_.getGlobalBounds().contains(pos))
    {
        model_.reset();
        return;
    }

    const auto& cards = model_.cards();
    for (int i = 0; i < static_cast<int>(cards.size()); ++i)
    {
        if (cards[i].contains(pos))
        {
            model_.tryFlip(i);
            return;
        }
    }
}

void GameScene::update(float dt)
{
    model_.update(dt);

    if (!fontLoaded_) return;

    {
        std::ostringstream oss;
        oss << "Moves: " << model_.moves();
        movesTxt_.setString(oss.str());
    }

    {
        std::ostringstream oss;
        oss << "Level: " << model_.level() << "  (" << model_.gridSize() << "x" << model_.gridSize() << ")";
        levelTxt_.setString(oss.str());
        const sf::FloatRect b = levelTxt_.getLocalBounds();
        levelTxt_.setOrigin(b.left + b.width, b.top);
        levelTxt_.setPosition(kWinWidth - 12.f, 10.f);
    }

    if (model_.state() == GameState::Won)
    {
        std::ostringstream oss;
        if (model_.gridSize() < model_.maxGridSize())
            oss << "Level Complete!  Next: " << (model_.gridSize() + 2) << "x" << (model_.gridSize() + 2);
        else
            oss << "Max Level Complete! Restart to play again";
        statusTxt_.setString(oss.str());
        const sf::FloatRect b = statusTxt_.getLocalBounds();
        statusTxt_.setOrigin(b.left + b.width / 2.f, b.top);
        statusTxt_.setPosition(kWinWidth / 2.f, kBtnY - 34.f);
    }
    else
    {
        statusTxt_.setString("");
    }

    {
        const sf::FloatRect b = restartTxt_.getLocalBounds();
        restartTxt_.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
        restartTxt_.setPosition(
            restartBtn_.getPosition().x + kBtnW / 2.f,
            restartBtn_.getPosition().y + kBtnH / 2.f);
    }
}

void GameScene::draw(sf::RenderWindow& window)
{
    window.clear(sf::Color(22, 28, 42));

    for (const auto& card : model_.cards())
        card.draw(window, fontLoaded_ ? &font_ : nullptr);

    window.draw(restartBtn_);

    if (fontLoaded_)
    {
        window.draw(titleTxt_);
        window.draw(movesTxt_);
        window.draw(levelTxt_);
        window.draw(restartTxt_);
        if (model_.state() == GameState::Won)
            window.draw(statusTxt_);
    }
}
