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
    unit_test.cpp
    game.test.cpp
)
target_link_libraries(unittest PUBLIC game Catch2)

add_executable(text-game text-game.cpp)
target_link_libraries(text-game PUBLIC game LibEdit)
