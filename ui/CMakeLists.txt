add_executable(scrabble-ui main.cpp)
target_link_libraries(scrabble-ui
    PUBLIC
        cxx_project_options
        GL3W
        ImGui
        fmt::fmt
)

add_library(game game.h game.cpp)
target_link_libraries(game PUBLIC cxx_project_options)

add_executable(unittest
    ${CATCH_MAIN}
    game.test.cpp
)
target_link_libraries(unittest PUBLIC game Catch2)

add_executable(text-game text-game.cpp engine.cpp)
target_link_libraries(text-game
    PUBLIC
        game
        LibEdit
        # re2::re2
        RE2
        fmt::fmt
        cxxopts::cxxopts
    )

add_executable(engine-test engine_test.cpp engine.cpp)
target_link_libraries(engine-test PUBLIC game)

add_executable(game-test game-test.cpp engine.cpp)
target_link_libraries(game-test
    PUBLIC
        cxx_project_options
        game
        fmt::fmt
        cxxopts::cxxopts
        # re2::re2
        RE2
)

add_executable(datrie datrie.cpp)
target_link_libraries(datrie
    PUBLIC
        cxx_project_options
)
