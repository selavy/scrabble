add_library(Mafsa++ mafsa.h mafsa.cpp mafsa_generated.h)
target_link_libraries(Mafsa++
    PUBLIC
        Mafsa
    PRIVATE
        cxx_project_options
        cxx_project_warnings
        ZLIB::ZLIB
)
target_include_directories(Mafsa++ PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(scrabble scrabble.cpp)
target_link_libraries(scrabble
    PUBLIC
        cxx_project_options
        cxx_project_warnings
        Mafsa++
)

add_executable(makedawg makedawg.cpp)
target_link_libraries(makedawg
    PUBLIC
        cxx_project_options
        cxx_project_warnings
        Mafsa++
)
