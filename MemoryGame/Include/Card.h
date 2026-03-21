#pragma once

#include <SFML/Graphics.hpp>

class Card
{
public:
    Card(int pairId, sf::Vector2f position, sf::Vector2f size);

    void flipUp();
    void flipDown();

    [[nodiscard]] bool isFaceUp()   const { return targetFaceUp_; }
    [[nodiscard]] bool isMatched()  const { return matched_; }
    [[nodiscard]] bool isAnimating() const;
    [[nodiscard]] int  getPairId()  const { return pairId_; }

    void setMatched(bool matched);

    [[nodiscard]] bool contains(sf::Vector2f point) const;
    [[nodiscard]] sf::FloatRect getBounds() const;

    void update(float dt);
    void draw(sf::RenderWindow& window, sf::Font* font) const;

private:
    static sf::Color pairColor(int pairId);

    int           pairId_;
    sf::Vector2f  position_;
    sf::Vector2f  size_;

    bool  matched_     { false };
    bool  targetFaceUp_{ false };

    float flipT_{ 0.f };
    static constexpr float kFlipSpeed{ 6.f };
};
