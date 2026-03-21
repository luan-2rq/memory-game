#include "Card.h"

#include <algorithm>
#include <cmath>
#include <string>

namespace
{
    sf::Color hsvToRgb(float h, float s, float v)
    {
        h = std::fmod(h, 360.f);
        if (h < 0.f)
            h += 360.f;

        const float c = v * s;
        const float x = c * (1.f - std::abs(std::fmod(h / 60.f, 2.f) - 1.f));
        const float m = v - c;

        float r = 0.f;
        float g = 0.f;
        float b = 0.f;

        if (h < 60.f)      { r = c; g = x; b = 0.f; }
        else if (h < 120.f){ r = x; g = c; b = 0.f; }
        else if (h < 180.f){ r = 0.f; g = c; b = x; }
        else if (h < 240.f){ r = 0.f; g = x; b = c; }
        else if (h < 300.f){ r = x; g = 0.f; b = c; }
        else               { r = c; g = 0.f; b = x; }

        return sf::Color(
            static_cast<sf::Uint8>((r + m) * 255.f),
            static_cast<sf::Uint8>((g + m) * 255.f),
            static_cast<sf::Uint8>((b + m) * 255.f));
    }

    std::string pairLabelForId(int pairId)
    {
        static const std::string symbols = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
        if (pairId >= 0 && pairId < static_cast<int>(symbols.size()))
            return std::string(1, symbols[static_cast<std::size_t>(pairId)]);

        const int base = static_cast<int>(symbols.size());
        int value = std::max(0, pairId);
        std::string encoded;
        do
        {
            encoded.push_back(symbols[static_cast<std::size_t>(value % base)]);
            value /= base;
        } while (value > 0);
        std::reverse(encoded.begin(), encoded.end());
        return encoded;
    }
}

Card::Card(int pairId, sf::Vector2f position, sf::Vector2f size)
    : pairId_(pairId), position_(position), size_(size)
{}

void Card::flipUp()
{
    targetFaceUp_ = true;
}

void Card::flipDown()
{
    if (!matched_)
        targetFaceUp_ = false;
}

bool Card::isAnimating() const
{
    const float target = (targetFaceUp_ || matched_) ? 1.f : 0.f;
    return std::abs(flipT_ - target) > 0.01f;
}

void Card::setMatched(bool matched)
{
    matched_ = matched;
    if (matched)
        targetFaceUp_ = true;
}

bool Card::contains(sf::Vector2f point) const
{
    return point.x >= position_.x && point.x <= position_.x + size_.x
        && point.y >= position_.y && point.y <= position_.y + size_.y;
}

sf::FloatRect Card::getBounds() const
{
    return { position_.x, position_.y, size_.x, size_.y };
}

void Card::update(float dt)
{
    const float target = (targetFaceUp_ || matched_) ? 1.f : 0.f;
    const float diff   = target - flipT_;
    const float step   = kFlipSpeed * dt;

    if (std::abs(diff) <= step)
        flipT_ = target;
    else
        flipT_ += (diff > 0.f ? step : -step);
}

void Card::draw(sf::RenderWindow& window, sf::Font* font) const
{
    const float angle   = (1.f - flipT_) * 3.14159f;
    const float scaleX  = std::abs(std::cos(angle));
    const bool  showFront = (flipT_ >= 0.5f);

    const float scaledW = scaleX * size_.x;
    const float offsetX = (size_.x - scaledW) * 0.5f;

    sf::RectangleShape body({ scaledW, size_.y });
    body.setPosition(position_.x + offsetX, position_.y);
    body.setOutlineThickness(2.f);
    body.setOutlineColor(sf::Color(220, 220, 220));

    if (matched_)
    {
        const sf::Color c = pairColor(pairId_);
        body.setFillColor(sf::Color(
            static_cast<sf::Uint8>(std::min(255, c.r + 55)),
            static_cast<sf::Uint8>(std::min(255, c.g + 55)),
            static_cast<sf::Uint8>(std::min(255, c.b + 55))));
    }
    else if (showFront)
    {
        body.setFillColor(pairColor(pairId_));
    }
    else
    {
        body.setFillColor(sf::Color(40, 60, 105));
    }

    window.draw(body);

    if (!showFront && scaleX > 0.25f)
    {
        const float margin = 9.f * scaleX;
        sf::RectangleShape inner({ scaledW - 2.f * margin, size_.y - 2.f * margin });
        inner.setPosition(position_.x + offsetX + margin, position_.y + margin);
        inner.setFillColor(sf::Color::Transparent);
        inner.setOutlineColor(sf::Color(90, 120, 175));
        inner.setOutlineThickness(2.f);
        window.draw(inner);
    }

    if (showFront && scaleX > 0.3f && font)
    {
        sf::Text label;
        label.setFont(*font);
        label.setString(pairLabelForId(pairId_));
        label.setCharacterSize(44);
        label.setStyle(sf::Text::Bold);
        label.setFillColor(sf::Color(255, 255, 255, 230));

        const sf::FloatRect lb = label.getLocalBounds();
        label.setOrigin(lb.left + lb.width / 2.f, lb.top + lb.height / 2.f);
        label.setPosition(position_.x + size_.x / 2.f, position_.y + size_.y / 2.f);
        window.draw(label);
    }
}

sf::Color Card::pairColor(int pairId)
{
    const float hue = std::fmod(static_cast<float>(pairId) * 137.508f, 360.f);
    return hsvToRgb(hue, 0.60f, 0.86f);
}
