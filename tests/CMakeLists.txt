add_executable(cicero_tests
    ${CATCH_MAIN}
    test_helpers.h
    mafsa.test.cpp
    square.test.cpp
    movegen.test.cpp
    score.test.cpp
    legality.test.cpp
    gcg.test.cpp
    recalc.test.cpp
)

target_link_libraries(cicero_tests
    PUBLIC
        Catch2
        # Mafsa
        Mafsa++
        Scrabble
        Cicero
        cxx_project_options
)

add_executable(game-replay-test game_replay_test.cpp)
target_link_libraries(game-replay-test
    PUBLIC
        Mafsa++
        Scrabble
        Cicero
        fmt::fmt
        cxxopts::cxxopts
        RE2
        cxx_project_options
    )

add_executable(find-best-move find_best_move.cpp)
target_link_libraries(find-best-move
    PUBLIC
        Mafsa++
        Scrabble
        Cicero
        fmt::fmt
        cxxopts::cxxopts
        RE2
        cxx_project_options
    )
