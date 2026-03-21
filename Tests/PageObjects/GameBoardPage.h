#pragma once

#include "Conditions/GameConditions.h"
#include "GameInteractions.h"
#include "E2EFramework/PageObject.h"

#include <string>

namespace MemoryGameTests
{

class GameBoardPage : public E2EFramework::PageObject
{
public:
    explicit GameBoardPage(E2EFramework::Session& session) : E2EFramework::PageObject(session) {}

    GameBoardPage& clickCard(int index)
    {
        const E2EFramework::ExecutionResult result = run(
            MemoryGameTests::ClickCard(index)
                .When(Conditions::GameIsPlaying())
                .ThenExpect(Conditions::CardIsFaceUp(index),
                    "card " + std::to_string(index) + " to be face up"));

        lastResult_ = result;
        return *this;
    }

    GameBoardPage& clickMatchingCard(int index, int expectedMatchCount)
    {
        const E2EFramework::ExecutionResult result = run(
            MemoryGameTests::ClickCard(index)
                .When(Conditions::GameIsPlaying())
                .ThenExpect(Conditions::MatchedPairsAtLeast(expectedMatchCount),
                    "pair to be matched"));

        lastResult_ = result;
        return *this;
    }

    GameBoardPage& restart()
    {
        const E2EFramework::ExecutionResult result = run(
            MemoryGameTests::RestartGame()
                .ThenExpect(Conditions::LevelIs(1),
                    "game to restart at level 1"));

        lastResult_ = result;
        return *this;
    }

    [[nodiscard]] bool isAtLevel(int level) const
    {
        return Conditions::asGameDriver(driver()).model().level() == level;
    }

    [[nodiscard]] bool hasGridSize(int size) const
    {
        return Conditions::asGameDriver(driver()).model().gridSize() == size;
    }

    [[nodiscard]] bool isInState(GameState s) const
    {
        return Conditions::asGameDriver(driver()).model().state() == s;
    }

    [[nodiscard]] int matchedPairs() const
    {
        return Conditions::asGameDriver(driver()).model().matchedPairs();
    }

    [[nodiscard]] int moves() const
    {
        return Conditions::asGameDriver(driver()).model().moves();
    }

    [[nodiscard]] int cardCount() const
    {
        return static_cast<int>(Conditions::asGameDriver(driver()).model().cards().size());
    }

    [[nodiscard]] const E2EFramework::ExecutionResult& lastResult() const
    {
        return lastResult_;
    }

private:
    E2EFramework::ExecutionResult lastResult_;
};

}
