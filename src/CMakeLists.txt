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

add_library(Scrabble scrabble.h scrabble.cpp)
target_link_libraries(Scrabble PRIVATE cxx_project_options cxx_project_warnings)
target_include_directories(Scrabble PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# -----------------------------------------------------------------------------

add_executable(play_cicero driver.cpp)
target_link_libraries(play_cicero
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
