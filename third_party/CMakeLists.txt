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

add_subdirectory(imgui)

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
# add_executable(target ${CATCH_MAIN})
# target_link_libraries(target PUBLIC Catch2)

option(BUILD_SHARED_LIBS "build shared libraries" OFF)
option(USEPCRE "use PCRE in tests and benchmarks" OFF)
option(RE2_BUILD_TESTING "enable testing for RE2" OFF)
add_subdirectory(re2) # target re2::re2

option(CXXOPTS_BUILD_EXAMPLES OFF)
option(CXXOPTS_BUILD_TESTS OFF)
option(CXXOPTS_ENABLE_INSTALL OFF)
add_subdirectory(cxxopts)
