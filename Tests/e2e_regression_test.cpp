#include "E2EFramework/TestCategories.h"
#include "e2e_support.h"

// =============================================================================
// Regression — full behavioural coverage.
// Run with:  ./memorygame_e2e --category=Regression
// =============================================================================

E2E_TEST(Board, CompletingLevel1AdvancesToLevel2, Regression)
{
    GameModelConfig cfg;
    cfg.winDelay = cfg.mismatchDelay = 0.f;
    Session session = makeSession(cfg);
    GameBoardPage board(session);

    ASSERT_TRUE(board.isAtLevel(1));
    matchAllPairs(session);

    EXPECT_TRUE(board.isAtLevel(2));
    EXPECT_TRUE(board.hasGridSize(6));
    EXPECT_TRUE(board.isInState(GameState::Playing));
}

E2E_TEST(Board, MismatchedPairResolvesWithoutCounting, Regression)
{
    GameModelConfig cfg;
    cfg.mismatchDelay = 0.f;
    Session session = makeSession(cfg);
    GameBoardPage board(session);

    const auto& cards = session.driver().model().cards();
    int a = -1, b = -1;
    for (int i = 0; i < static_cast<int>(cards.size()) && b == -1; ++i)
        for (int j = i + 1; j < static_cast<int>(cards.size()) && b == -1; ++j)
            if (cards[i].getPairId() != cards[j].getPairId())
                { a = i; b = j; }

    board.clickCard(a);
    ASSERT_TRUE(board.lastResult().passed) << board.lastResult().message;

    const ExecutionResult result = session.Run(
        E2EFramework::Interaction::Click(b)
            .When(When::GameIsPlaying())
            .Do()
            .ThenExpect(Expect::StateIs(GameState::Playing),
                "mismatch to auto-resolve back to Playing"));

    ASSERT_TRUE(result.passed) << result.message;
    EXPECT_EQ(board.matchedPairs(), 0);
}

E2E_TEST(Board, RestartMidGameResetsProgress, Regression)
{
    GameModelConfig cfg;
    cfg.winDelay = cfg.mismatchDelay = 0.f;
    Session session = makeSession(cfg);
    GameBoardPage board(session);

    matchAllPairs(session);
    ASSERT_TRUE(board.isAtLevel(2));

    board.restart();
    ASSERT_TRUE(board.lastResult().passed) << board.lastResult().message;

    EXPECT_TRUE(board.isAtLevel(1));
    EXPECT_TRUE(board.hasGridSize(4));
    EXPECT_EQ(board.moves(), 0);
    EXPECT_TRUE(board.isInState(GameState::Playing));
}

E2E_TEST(Board, MultiLevelProgression, Regression)
{
    GameModelConfig cfg;
    cfg.winDelay = cfg.mismatchDelay = 0.f;
    Session session = makeSession(cfg);
    GameBoardPage board(session);

    ASSERT_TRUE(board.hasGridSize(4));
    matchAllPairs(session);

    ASSERT_TRUE(board.isAtLevel(2));
    ASSERT_TRUE(board.hasGridSize(6));
    matchAllPairs(session);

    EXPECT_TRUE(board.isAtLevel(3));
    EXPECT_TRUE(board.hasGridSize(8));
}
