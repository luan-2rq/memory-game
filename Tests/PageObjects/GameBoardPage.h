#pragma once

#include "E2EFramework/PageObject.h"

#include <string>

namespace MemoryGameTests
{

namespace Expect
{

inline E2EFramework::Interaction::Predicate CardIsFaceUp(int index)
{
    return [index](const GameDriver& d)
    {
        return d.model().cards()[index].isFaceUp();
    };
}

inline E2EFramework::Interaction::Predicate StateIs(GameState expected)
{
    return [expected](const GameDriver& d)
    {
        return d.model().state() == expected;
    };
}

inline E2EFramework::Interaction::Predicate MatchedPairsEq(int count)
{
    return [count](const GameDriver& d)
    {
        return d.model().matchedPairs() == count;
    };
}

inline E2EFramework::Interaction::Predicate MatchedPairsAtLeast(int count)
{
    return [count](const GameDriver& d)
    {
        return d.model().matchedPairs() >= count;
    };
}

inline E2EFramework::Interaction::Predicate LevelIs(int level)
{
    return [level](const GameDriver& d)
    {
        return d.model().level() == level;
    };
}

inline E2EFramework::Interaction::Predicate GridSizeIs(int size)
{
    return [size](const GameDriver& d)
    {
        return d.model().gridSize() == size;
    };
}

inline E2EFramework::Interaction::Predicate MovesEq(int moves)
{
    return [moves](const GameDriver& d)
    {
        return d.model().moves() == moves;
    };
}

inline E2EFramework::Interaction::Predicate WindowOpen()
{
    return [](const GameDriver& d)
    {
        return d.isRunning();
    };
}

}  // namespace Expect

namespace When
{

inline E2EFramework::Interaction::Predicate GameIsPlaying()
{
    return [](const GameDriver& d)
    {
        return d.model().state() == GameState::Playing;
    };
}

inline E2EFramework::Interaction::Predicate WindowIsOpen()
{
    return [](const GameDriver& d)
    {
        return d.isRunning();
    };
}

inline E2EFramework::Interaction::Predicate CardIsFaceDown(int index)
{
    return [index](const GameDriver& d)
    {
        const auto& card = d.model().cards()[index];
        return !card.isFaceUp() && !card.isMatched();
    };
}

}  // namespace When

class GameBoardPage : public E2EFramework::PageObject
{
public:
    explicit GameBoardPage(E2EFramework::Session& session) : E2EFramework::PageObject(session) {}

    GameBoardPage& clickCard(int index)
    {
        const E2EFramework::ExecutionResult result = run(
            E2EFramework::Interaction::Click(index)
                .When(When::GameIsPlaying())
                .Do()
                .ThenExpect(Expect::CardIsFaceUp(index),
                    "card " + std::to_string(index) + " to be face up"));

        lastResult_ = result;
        return *this;
    }

    GameBoardPage& clickMatchingCard(int index, int expectedMatchCount)
    {
        const E2EFramework::ExecutionResult result = run(
            E2EFramework::Interaction::Click(index)
                .When(When::GameIsPlaying())
                .Do()
                .ThenExpect(Expect::MatchedPairsAtLeast(expectedMatchCount),
                    "pair to be matched"));

        lastResult_ = result;
        return *this;
    }

    GameBoardPage& restart()
    {
        const E2EFramework::ExecutionResult result = run(
            E2EFramework::Interaction::Restart()
                .Do()
                .ThenExpect(Expect::LevelIs(1),
                    "game to restart at level 1"));

        lastResult_ = result;
        return *this;
    }

    [[nodiscard]] bool isAtLevel(int level) const
    {
        return driver().model().level() == level;
    }

    [[nodiscard]] bool hasGridSize(int size) const
    {
        return driver().model().gridSize() == size;
    }

    [[nodiscard]] bool isInState(GameState s) const
    {
        return driver().model().state() == s;
    }

    [[nodiscard]] int matchedPairs() const
    {
        return driver().model().matchedPairs();
    }

    [[nodiscard]] int moves() const
    {
        return driver().model().moves();
    }

    [[nodiscard]] int cardCount() const
    {
        return static_cast<int>(driver().model().cards().size());
    }

    [[nodiscard]] const E2EFramework::ExecutionResult& lastResult() const
    {
        return lastResult_;
    }

private:
    E2EFramework::ExecutionResult lastResult_;
};

}  // namespace MemoryGameTests
