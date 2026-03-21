#pragma once

#include "Drivers/GameDriver.h"
#include "E2EFramework/Interaction.h"
#include "Conditions/GameConditions.h"
#include "PageObjects/GameBoardPage.h"

#include <gtest/gtest.h>

#include <memory>
#include <vector>

using E2EFramework::ExecutionResult;
using E2EFramework::Session;
using E2EFramework::SessionConfig;
using MemoryGameTests::GameBoardPage;
namespace Conditions = MemoryGameTests::Conditions;
namespace Expect = MemoryGameTests::Conditions;
namespace When   = MemoryGameTests::Conditions;

static Session makeSession(GameModelConfig cfg = GameModelConfig{})
{
#ifdef MEMORYGAME_HEADED_E2E
    const MemoryGameTests::GameDriver::Mode driverMode = MemoryGameTests::GameDriver::Mode::Headed;
#else
    const MemoryGameTests::GameDriver::Mode driverMode = MemoryGameTests::GameDriver::Mode::Headless;
#endif
    SessionConfig config;
    config.driverFactory = [driverMode, cfg]()
    {
        return std::make_unique<MemoryGameTests::GameDriver>(driverMode, cfg);
    };
    config.retryPolicy.timeoutSeconds = 3.f;
    config.retryPolicy.pollIntervalSeconds = 1.f / 120.f;
    config.bootstrap = [](E2EFramework::Driver& driver)
    {
        driver.advance(0.016f);
    };
    return Session(config);
}

static void matchAllPairs(Session& session)
{
    GameBoardPage board(session);
    const int n = board.cardCount();

    std::vector<std::pair<int, int>> pairs;
    std::vector<bool> used(n, false);

    const auto& cards = Conditions::asGameDriver(session.driver()).model().cards();
    for (int i = 0; i < n; ++i)
    {
        if (used[i]) continue;
        for (int j = i + 1; j < n; ++j)
        {
            if (!used[j] && cards[j].getPairId() == cards[i].getPairId())
            {
                pairs.push_back({ i, j });
                used[i] = used[j] = true;
                break;
            }
        }
    }

    for (const auto& pair : pairs)
    {
        const int expectedMatches = board.matchedPairs() + 1;
        board.clickCard(pair.first);
        ASSERT_TRUE(board.lastResult().passed) << board.lastResult().message;
        board.clickMatchingCard(pair.second, expectedMatches);
        ASSERT_TRUE(board.lastResult().passed) << board.lastResult().message;
        Conditions::asGameDriver(session.driver()).settle();
    }
}
