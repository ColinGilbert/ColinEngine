cmake_minimum_required(VERSION 3.0)

project(esWrapper)

include_directories(glad/build/include)

add_subdirectory(glfw)

add_executable(glfwWrapper glfwWrapper.c)
target_link_libraries(glfwWrapper glfw ${GLFW_LIBRARIES})
