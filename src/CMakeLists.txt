add_executable(scrabble
    tables.h
    scrabble.cpp
)
target_link_libraries(scrabble
    PUBLIC
        cxx_project_options project_warnings
)
