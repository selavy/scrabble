add_executable(scrabble-ui main.cpp)
target_link_libraries(scrabble-ui
    PUBLIC
        cxx_project_options
        GL3W
        ImGui
        fmt::fmt
)
