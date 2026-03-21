include(GoogleTest)

# ---------------------------------------------------------------------------
# End-to-end tests
# Headless by default.  Set MEMORYGAME_HEADED_E2E=ON to enable the headed
# tests (requires a display / desktop environment).
# ---------------------------------------------------------------------------
option(MEMORYGAME_HEADED_E2E "Enable headed-window e2e tests (requires display)" OFF)

add_executable(memorygame_e2e
    e2e_main.cpp
    e2e_smoke_test.cpp
    e2e_regression_test.cpp
    Drivers/GameDriver.cpp
)

target_include_directories(memorygame_e2e PRIVATE
    ${CMAKE_SOURCE_DIR}/Engine/Include
    ${CMAKE_SOURCE_DIR}/MemoryGame/Include
    ${CMAKE_SOURCE_DIR}/E2EFramework/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(memorygame_e2e PRIVATE
    memorygame_e2e_framework
    MemoryGameCore
    GTest::gtest
)

if(MEMORYGAME_HEADED_E2E)
    target_compile_definitions(memorygame_e2e PRIVATE MEMORYGAME_HEADED_E2E)
endif()

gtest_discover_tests(memorygame_e2e)
