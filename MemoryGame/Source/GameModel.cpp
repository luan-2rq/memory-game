#include "GameModel.h"
#include "GameConstants.h"

#include <algorithm>
#include <random>

static constexpr float kMinCardSize  = 46.f;
static constexpr float kGap          = 10.f;
static constexpr float kBoardTop     = 68.f;
static constexpr float kBoardBottom  = 545.f;
static constexpr float kBoardMargin  = 22.f;

GameModel::GameModel(Config cfg)
    : cfg_(cfg), gridSize_(cfg.initialGridSize)
{
    initCards();
}

bool GameModel::tryFlip(int index)
{
    if (state_ != GameState::Playing) return false;

    Card& card = cards_[index];
    if (card.isMatched() || card.isFaceUp() || card.isAnimating()) return false;
    if (index == firstFlipped_ || index == secondFlipped_)         return false;

    card.flipUp();

    if (firstFlipped_ == -1)
    {
        firstFlipped_ = index;
    }
    else
    {
        secondFlipped_ = index;
        ++moves_;

        if (cards_[firstFlipped_].getPairId() == cards_[secondFlipped_].getPairId())
        {
            cards_[firstFlipped_].setMatched(true);
            cards_[secondFlipped_].setMatched(true);
            ++matchedPairs_;
            firstFlipped_  = -1;
            secondFlipped_ = -1;

            if (matchedPairs_ == pairCount_)
            {
                state_ = GameState::Won;
                winAction_.start(cfg_.winDelay, [this]()
                {
                    advanceLevel();
                });
            }
        }
        else
        {
            state_ = GameState::ShowingMismatch;
            mismatchAction_.start(cfg_.mismatchDelay, [this]()
            {
                cards_[firstFlipped_].flipDown();
                cards_[secondFlipped_].flipDown();
                firstFlipped_  = -1;
                secondFlipped_ = -1;
                state_         = GameState::Playing;
            });
        }
    }

    return true;
}

void GameModel::update(float dt)
{
    for (auto& card : cards_)
        card.update(dt);

    const bool canAdvance = !selectedCardsAnimating();
    mismatchAction_.update(dt, canAdvance);
    winAction_.update(dt, canAdvance);
}

void GameModel::reset()
{
    level_    = 1;
    gridSize_ = cfg_.initialGridSize;
    initCards();
}

void GameModel::initCards()
{
    cards_.clear();
    firstFlipped_  = -1;
    secondFlipped_ = -1;
    state_         = GameState::Playing;
    mismatchAction_.cancel();
    winAction_.cancel();
    moves_         = 0;
    matchedPairs_  = 0;

    pairCount_ = (gridSize_ * gridSize_) / 2;

    std::vector<int> ids;
    ids.reserve(gridSize_ * gridSize_);
    for (int i = 0; i < pairCount_; ++i) { ids.push_back(i); ids.push_back(i); }
    std::shuffle(ids.begin(), ids.end(), std::mt19937{ std::random_device{}() });

    const float boardWidth    = MemoryGame::kWindowWidthF - 2.f * kBoardMargin;
    const float boardHeight   = kBoardBottom - kBoardTop;
    const float cardW         = (boardWidth  - (gridSize_ - 1) * kGap) / static_cast<float>(gridSize_);
    const float cardH         = (boardHeight - (gridSize_ - 1) * kGap) / static_cast<float>(gridSize_);
    const float cardSize      = std::max(kMinCardSize, std::min(cardW, cardH));
    const float gridPixelSize = gridSize_ * cardSize + (gridSize_ - 1) * kGap;
    const float gridLeft      = (MemoryGame::kWindowWidthF - gridPixelSize) * 0.5f;
    const float gridTop       = kBoardTop + (boardHeight - gridPixelSize) * 0.5f;

    for (int row = 0; row < gridSize_; ++row)
    {
        for (int col = 0; col < gridSize_; ++col)
        {
            const int idx = row * gridSize_ + col;
            const sf::Vector2f pos(
                gridLeft + col * (cardSize + kGap),
                gridTop  + row * (cardSize + kGap));
            cards_.emplace_back(ids[idx], pos, sf::Vector2f(cardSize, cardSize));
        }
    }
}

void GameModel::advanceLevel()
{
    if (gridSize_ < cfg_.maxGridSize)
    {
        ++level_;
        gridSize_ += 2;
    }
    initCards();
}

bool GameModel::selectedCardsAnimating() const
{
    return (firstFlipped_  >= 0 && cards_[firstFlipped_].isAnimating())
        || (secondFlipped_ >= 0 && cards_[secondFlipped_].isAnimating());
}
