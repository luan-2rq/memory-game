#pragma once

#include "Drivers/GameDriver.h"
#include "E2EFramework/Interaction.h"

#include <string>

namespace MemoryGameTests
{

inline E2EFramework::Interaction ClickCard(int index)
{
    return E2EFramework::Interaction::Custom(
        "Click card " + std::to_string(index),
        [index](E2EFramework::Driver& d)
        {
            static_cast<GameDriver&>(d).clickCard(index);
        });
}

inline E2EFramework::Interaction RestartGame()
{
    return E2EFramework::Interaction::Custom(
        "Restart",
        [](E2EFramework::Driver& d)
        {
            static_cast<GameDriver&>(d).clickRestart();
        });
}

}  // namespace MemoryGameTests
