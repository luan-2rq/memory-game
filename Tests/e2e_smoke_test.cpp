#include "E2EFramework/TestCategories.h"
#include "e2e_support.h"

// =============================================================================
// Smoke — the absolute minimum that must pass before any other test runs.
// Run with:  ./memorygame_e2e --category=Smoke
// =============================================================================

E2E_TEST(Board, GameStartsAtLevelOne, Smoke)
{
    GameModelConfig cfg;
    cfg.winDelay = cfg.mismatchDelay = 0.f;
    Session session = makeSession(cfg);
    GameBoardPage board(session);

    EXPECT_TRUE(board.isAtLevel(1));
    EXPECT_TRUE(board.hasGridSize(4));
    EXPECT_EQ(board.cardCount(), 16);
    EXPECT_TRUE(board.isInState(GameState::Playing));
}

E2E_TEST(Board, ClickingCardFlipsItFaceUp, Smoke)
{
    GameModelConfig cfg;
    cfg.winDelay = cfg.mismatchDelay = 0.f;
    Session session = makeSession(cfg);
    GameBoardPage board(session);

    board.clickCard(0);

    ASSERT_TRUE(board.lastResult().passed) << board.lastResult().message;
    EXPECT_TRUE(session.driver().model().cards()[0].isFaceUp());
}

E2E_TEST(Board, RestartResetsGame, Smoke)
{
    GameModelConfig cfg;
    cfg.winDelay = cfg.mismatchDelay = 0.f;
    Session session = makeSession(cfg);
    GameBoardPage board(session);

    board.clickCard(0);
    ASSERT_TRUE(board.lastResult().passed) << board.lastResult().message;

    board.restart();
    ASSERT_TRUE(board.lastResult().passed) << board.lastResult().message;

    EXPECT_TRUE(board.isAtLevel(1));
    EXPECT_EQ(board.moves(), 0);
    EXPECT_TRUE(board.isInState(GameState::Playing));
}
