#pragma once

#include "E2EFramework/GameDriver.h"
#include "E2EFramework/Interaction.h"

namespace MemoryGameTests
{
namespace Conditions
{

using Condition = E2EFramework::Interaction::Predicate;

inline Condition CardIsFaceUp(int index)
{
    return [index](const GameDriver& d)
    {
        return d.model().cards()[index].isFaceUp();
    };
}

inline Condition StateIs(GameState expected)
{
    return [expected](const GameDriver& d)
    {
        return d.model().state() == expected;
    };
}

inline Condition MatchedPairsEq(int count)
{
    return [count](const GameDriver& d)
    {
        return d.model().matchedPairs() == count;
    };
}

inline Condition MatchedPairsAtLeast(int count)
{
    return [count](const GameDriver& d)
    {
        return d.model().matchedPairs() >= count;
    };
}

inline Condition LevelIs(int level)
{
    return [level](const GameDriver& d)
    {
        return d.model().level() == level;
    };
}

inline Condition GridSizeIs(int size)
{
    return [size](const GameDriver& d)
    {
        return d.model().gridSize() == size;
    };
}

inline Condition MovesEq(int moves)
{
    return [moves](const GameDriver& d)
    {
        return d.model().moves() == moves;
    };
}

inline Condition WindowOpen()
{
    return [](const GameDriver& d)
    {
        return d.isRunning();
    };
}

inline Condition GameIsPlaying()
{
    return [](const GameDriver& d)
    {
        return d.model().state() == GameState::Playing;
    };
}

inline Condition WindowIsOpen()
{
    return [](const GameDriver& d)
    {
        return d.isRunning();
    };
}

inline Condition CardIsFaceDown(int index)
{
    return [index](const GameDriver& d)
    {
        const auto& card = d.model().cards()[index];
        return !card.isFaceUp() && !card.isMatched();
    };
}

}  // namespace Conditions
}  // namespace MemoryGameTests
