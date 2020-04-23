add_executable(cicero_tests
    ${CATCH_MAIN}
    mafsa.test.cpp
    movegen.test.cpp
)

target_link_libraries(cicero_tests
    PUBLIC
        Catch2
        Mafsa
        Mafsa++
        Scrabble
        Cicero
        cxx_project_options
)
