#ifndef GLFW_WRAPPERS
#define GLFW_WRAPPERS

#include <stdlib.h>
#include <stdio.h>
//#include <glad/glad.h>
#include <GLFW/glfw3.h>

void button_callback(GLFWwindow* win, int bt, int action, int mods);
void cursor_callback(GLFWwindow* win, double x, double y);
void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods);
void char_callback(GLFWwindow* win, unsigned int key);
void error_callback(int err, const char* desc);
void resize_callback(GLFWwindow* window, int width, int height);
int main() {
	glfwSetErrorCallback(error_callback);
	if(!glfwInit()) {
		printf("Error: cannot setup glfw.\n");
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* win = NULL;
	int w = 1280;
	int h = 720;
	win = glfwCreateWindow(w, h, "GLFW", NULL, NULL);
	if(!win) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetFramebufferSizeCallback(win, resize_callback);
	glfwSetKeyCallback(win, key_callback);
	glfwSetCharCallback(win, char_callback);
	glfwSetCursorPosCallback(win, cursor_callback);
	glfwSetMouseButtonCallback(win, button_callback);
	glfwMakeContextCurrent(win);
	glfwSwapInterval(1);

	/*if (!gladLoadGL()) {
		printf("Cannot load GL.\n");
		exit(1);
	}*/

	// ----------------------------------------------------------------
	// THIS IS WHERE YOU START CALLING OPENGL FUNCTIONS, NOT EARLIER!!
	// ----------------------------------------------------------------
	while(!glfwWindowShouldClose(win)) {
		glClearColor(0.2f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwSwapBuffers(win);
		glfwPollEvents();
	}
	glfwTerminate();
	return EXIT_SUCCESS;
}

void char_callback(GLFWwindow* win, unsigned int key) {
}
void key_callback(GLFWwindow* win, int key, int scancode, int action, int mods) {
	if(action != GLFW_PRESS) {
		return;
	}
	switch(key) {
		case GLFW_KEY_ESCAPE: {
					      glfwSetWindowShouldClose(win, GL_TRUE);
					      break;
				      }
	};
}
void resize_callback(GLFWwindow* window, int width, int height) {
}
void cursor_callback(GLFWwindow* win, double x, double y) {
}
void button_callback(GLFWwindow* win, int bt, int action, int mods) {
	/*
	   if(action == GLFW_PRESS) {
	   }
	   */
}
void error_callback(int err, const char* desc) {
	printf("GLFW error: %s (%d)\n", desc, err);
} 
#endif
