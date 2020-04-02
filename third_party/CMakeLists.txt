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
