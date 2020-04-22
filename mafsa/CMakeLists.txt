add_library(Mafsa
    include/mafsa/mafsa.h
    src/iconv.h
    src/mafsa.c
    src/buildmafsa.c
)
target_link_libraries(Mafsa PRIVATE c_project_warnings c_project_options)
target_include_directories(Mafsa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
