cmake_minimum_required(VERSION 3.0)

project(gameEngine)

add_subdirectory(glfw)

add_executable(editor desktop.c)
target_link_libraries(editor glfw ${GLFW_LIBRARIES})
