add_library(Mafsa
    iconv.h
    mafsa.h
    mafsa.c
)
target_link_libraries(Mafsa PRIVATE c_project_warnings c_project_options)

add_executable(scrabble
    scrabble.cpp
)
target_link_libraries(scrabble
    PUBLIC
        cxx_project_options cxx_project_warnings
        Mafsa
)
