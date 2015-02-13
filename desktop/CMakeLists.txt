cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -frtti -fexceptions")
set(CMAKE_C_FLAGS "-std=c99")

project(gameEngine)

add_subdirectory(engine)

add_executable(editor desktop.c) 
target_link_libraries(editor glfw ${GLFW_LIBRARIES} enet ${ENET_LIBRARIES} )
	#freetype ${FREETYPE_LIBRARIES})
