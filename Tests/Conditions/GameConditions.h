#pragma once

#include "Drivers/GameDriver.h"
#include "E2EFramework/Conditions.h"
#include "E2EFramework/Interaction.h"

namespace MemoryGameTests
{
namespace Conditions
{

using Condition = E2EFramework::Conditions::Condition;

inline const MemoryGameTests::GameDriver& asGameDriver(const E2EFramework::Driver& driver)
{
    return static_cast<const MemoryGameTests::GameDriver&>(driver);
}

inline MemoryGameTests::GameDriver& asGameDriver(E2EFramework::Driver& driver)
{
    return static_cast<MemoryGameTests::GameDriver&>(driver);
}

inline Condition CardIsFaceUp(int index)
{
    return [index](const E2EFramework::Driver& d)
    {
        return asGameDriver(d).model().cards()[index].isFaceUp();
    };
}

inline Condition StateIs(GameState expected)
{
    return [expected](const E2EFramework::Driver& d)
    {
        return asGameDriver(d).model().state() == expected;
    };
}

inline Condition MatchedPairsEq(int count)
{
    return [count](const E2EFramework::Driver& d)
    {
        return asGameDriver(d).model().matchedPairs() == count;
    };
}

inline Condition MatchedPairsAtLeast(int count)
{
    return [count](const E2EFramework::Driver& d)
    {
        return asGameDriver(d).model().matchedPairs() >= count;
    };
}

inline Condition LevelIs(int level)
{
    return [level](const E2EFramework::Driver& d)
    {
        return asGameDriver(d).model().level() == level;
    };
}

inline Condition GridSizeIs(int size)
{
    return [size](const E2EFramework::Driver& d)
    {
        return asGameDriver(d).model().gridSize() == size;
    };
}

inline Condition MovesEq(int moves)
{
    return [moves](const E2EFramework::Driver& d)
    {
        return asGameDriver(d).model().moves() == moves;
    };
}

inline Condition WindowOpen()
{
    return E2EFramework::Conditions::DriverIsRunning();
}

inline Condition GameIsPlaying()
{
    return [](const E2EFramework::Driver& d)
    {
        return asGameDriver(d).model().state() == GameState::Playing;
    };
}

inline Condition WindowIsOpen()
{
    return E2EFramework::Conditions::DriverIsRunning();
}

inline Condition CardIsFaceDown(int index)
{
    return [index](const E2EFramework::Driver& d)
    {
        const auto& card = asGameDriver(d).model().cards()[index];
        return !card.isFaceUp() && !card.isMatched();
    };
}

}  // namespace Conditions
}  // namespace MemoryGameTests
