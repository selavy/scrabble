add_executable(scrabble-ui main.cpp)
target_link_libraries(scrabble-ui
    PUBLIC
        cxx_project_options
        GL3W
        ImGui
        fmt::fmt
)

# add_executable(game game.cpp)
add_executable(game
    unit_test.cpp
    game.h
    game.cpp
    game.test.cpp
)
target_link_libraries(game PUBLIC cxx_project_options Catch2)
