# LibFMT
add_subdirectory(fmt)

# GL3W
add_library(GL3W ./gl3w/GL/gl3w.c)
target_include_directories(GL3W PUBLIC gl3w)

# OpenGL
set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(OpenGL REQUIRED)

# GLFW3
option(BUILD_SHARED_LIBS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_INSTALL OFF)
option(GLFW_VULKAN_STATIC OFF)
add_subdirectory(glfw)

# add_subdirectory(imgui)
add_library(ImGui
    ./imgui/imgui.cpp
    ./imgui/imgui_demo.cpp
    ./imgui/imgui_draw.cpp
    ./imgui/examples/imgui_impl_glfw.cpp
    ./imgui/examples/imgui_impl_opengl3.cpp
    ./imgui/imgui_widgets.cpp
    # ./imgui/imconfig.h
    # ./imgui/imgui.h
    # ./imgui/examples/imgui_impl_glfw.h
    # ./imgui/examples/imgui_impl_opengl3.h
    # ./imgui/imgui_internal.h
    # ./imgui/imstb_rectpack.h
    # ./imgui/imstb_textedit.h
    # ./imgui/imstb_truetype.h
)
target_compile_definitions(ImGui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GL3W)
target_include_directories(ImGui PUBLIC imgui imgui/examples)
target_link_libraries(ImGui
    PUBLIC
        cxx_project_options
        OpenGL::GL
        glfw
        GL3W
    )

option(CATCH_USE_VALGRIND "Perform SelfTests with Valgrind" OFF)
option(CATCH_BUILD_TESTING "Build SelfTest project" OFF)
option(CATCH_BUILD_EXAMPLES "Build documentation examples" OFF)
option(CATCH_BUILD_EXTRA_TESTS "Build extra tests" OFF)
option(CATCH_ENABLE_COVERAGE "Generate coverage for codecov.io" OFF)
option(CATCH_ENABLE_WERROR "Enable all warnings as errors" OFF)
option(CATCH_INSTALL_DOCS "Install documentation alongside library" OFF)
option(CATCH_INSTALL_HELPERS "Install contrib alongside library" OFF)
add_subdirectory(Catch2)
set(CATCH_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/unit_test_main.cpp" PARENT_SCOPE)

option(BUILD_SHARED_LIBS "build shared libraries" OFF)
option(USEPCRE "use PCRE in tests and benchmarks" OFF)
option(RE2_BUILD_TESTING "enable testing for RE2" OFF)
add_subdirectory(re2) # target re2::re2
add_library(RE2 INTERFACE)
target_link_libraries(RE2 INTERFACE re2::re2 Threads::Threads)
target_include_directories(RE2 INTERFACE ./re2)

option(CXXOPTS_BUILD_EXAMPLES OFF)
option(CXXOPTS_BUILD_TESTS OFF)
option(CXXOPTS_ENABLE_INSTALL OFF)
add_subdirectory(cxxopts)
