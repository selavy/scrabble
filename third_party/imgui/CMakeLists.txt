add_library(ImGui
    src/imgui.cpp
    src/imgui_demo.cpp
    src/imgui_draw.cpp
    src/imgui_impl_glfw.cpp
    src/imgui_impl_opengl3.cpp
    src/imgui_widgets.cpp

    include/imconfig.h
    include/imgui.h
    include/imgui_impl_glfw.h
    include/imgui_impl_opengl3.h
    include/imgui_internal.h
    include/imstb_rectpack.h
    include/imstb_textedit.h
    include/imstb_truetype.h
)
target_compile_definitions(ImGui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GL3W)
target_include_directories(ImGui PUBLIC include)
target_link_libraries(ImGui
    PUBLIC
        cxx_project_options
        OpenGL::GL
        glfw
        GL3W
    )
