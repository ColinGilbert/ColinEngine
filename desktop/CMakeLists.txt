cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -frtti -fexceptions")
set(CMAKE_C_FLAGS "-std=c99")

project(gameEngine)

include_directories(./engine/extern/nanovg/src)

add_subdirectory(engine)

add_executable(editor desktop.cpp) #./engine/extern/nanovg/stb_image.h) 
target_link_libraries(editor glfw ${GLFW_LIBRARIES} enet ${ENET_LIBRARIES} )
