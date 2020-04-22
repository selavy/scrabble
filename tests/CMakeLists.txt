
add_executable(cicero_tests
    ${CATCH_MAIN}
    mafsa.test.cpp
)

target_link_libraries(cicero_tests
    PUBLIC
        Catch2
        Mafsa
        cxx_project_options
)
