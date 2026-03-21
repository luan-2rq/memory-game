add_library(MemoryGameCore
    Source/Card.cpp
    Source/GameModel.cpp
    Source/GameScene.cpp
)

target_include_directories(MemoryGameCore PUBLIC Include)

target_link_libraries(MemoryGameCore PUBLIC Engine)

add_executable(MemoryGame
    Source/main.cpp
)

target_include_directories(MemoryGame PRIVATE Include)

target_link_libraries(MemoryGame PRIVATE MemoryGameCore)

if(APPLE)
    set_target_properties(MemoryGame PROPERTIES
        MACOSX_BUNDLE FALSE
    )
endif()