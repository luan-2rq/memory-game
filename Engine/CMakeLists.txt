file(GLOB_RECURSE SOURCES "Source/*.cpp")

add_library(Engine ${SOURCES})

target_include_directories(Engine PUBLIC Include)

target_link_libraries(Engine PUBLIC ${SFML_LINK_TARGETS})
