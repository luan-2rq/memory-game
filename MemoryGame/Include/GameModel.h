#pragma once

#include "Card.h"
#include "Engine/DelayedAction.h"

#include <vector>

enum class GameState
{
    Playing,
    ShowingMismatch,
    Won
};

struct GameModelConfig
{
    int   initialGridSize{ 4 };
    int   maxGridSize    { 8 };
    float mismatchDelay  { 0.9f };
    float winDelay       { 1.2f };
};

class GameModel
{
public:
    using Config = GameModelConfig;

    explicit GameModel(Config cfg = Config{});

    // Returns false if the flip was not allowed (wrong state, already matched, etc.)
    bool tryFlip(int index);

    void update(float dt);

    void reset();

    [[nodiscard]] const std::vector<Card>& cards()        const { return cards_; }
    [[nodiscard]] std::vector<Card>&       cards()              { return cards_; }
    [[nodiscard]] GameState                state()        const { return state_; }
    [[nodiscard]] int                      moves()        const { return moves_; }
    [[nodiscard]] int                      level()        const { return level_; }
    [[nodiscard]] int                      gridSize()     const { return gridSize_; }
    [[nodiscard]] int                      pairCount()    const { return pairCount_; }
    [[nodiscard]] int                      matchedPairs() const { return matchedPairs_; }
    [[nodiscard]] int                      maxGridSize()  const { return cfg_.maxGridSize; }

private:
    void initCards();
    void advanceLevel();
    [[nodiscard]] bool selectedCardsAnimating() const;

    Config cfg_;

    std::vector<Card> cards_;
    int firstFlipped_ { -1 };
    int secondFlipped_{ -1 };

    GameState state_  { GameState::Playing };
    int moves_        { 0 };
    int matchedPairs_ { 0 };
    int level_        { 1 };
    int gridSize_;
    int pairCount_    { 0 };

    Engine::DelayedAction mismatchAction_;
    Engine::DelayedAction winAction_;
};
