add_library(Cicero
    include/cicero/cicero.h
    src/cicero_types.h
    src/cicero_tables.h
    src/init.c
    src/rack.c
    src/movegen.c
    src/score.c
    src/legal.c
)
target_link_libraries(Cicero
    PRIVATE
    c_project_options
    # c_project_warnings
)
target_include_directories(Cicero PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
