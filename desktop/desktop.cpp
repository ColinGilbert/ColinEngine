#ifndef DESKTOP
#define DESKTOP

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include "platform_crap.h"
#include "user_callbacks.h"

#define GL_LOG_FILE "gl.log"
#define MAX_SHADER_LENGTH 262144

bool restart_gl_log () {
	FILE* file = fopen (GL_LOG_FILE, "w");
	if (!file) {
		fprintf (
			stderr,
			"ERROR: could not open GL_LOG_FILE log file %s for writing\n",
			GL_LOG_FILE
		);
		return false;
	}
	time_t now = time (NULL);
	char* date = ctime (&now);
	fprintf (file, "GL_LOG_FILE log. local time %s\n", date);
	fclose (file);
	return true;
}

bool gl_log (const char* message, ...) {
	va_list argptr;
	FILE* file = fopen (GL_LOG_FILE, "a");
	if (!file) {
		fprintf (
			stderr,
			"ERROR: could not open GL_LOG_FILE %s file for appending\n",
			GL_LOG_FILE
		);
		return false;
	}
	va_start (argptr, message);
	vfprintf (file, message, argptr);
	va_end (argptr);
	fclose (file);
	return true;
}

/* same as gl_log except also prints to stderr */
bool gl_log_err (const char* message, ...) {
	va_list argptr;
	FILE* file = fopen (GL_LOG_FILE, "a");
	if (!file) {
		fprintf (
			stderr,
			"ERROR: could not open GL_LOG_FILE %s file for appending\n",
			GL_LOG_FILE
		);
		return false;
	}
	va_start (argptr, message);
	vfprintf (file, message, argptr);
	va_end (argptr);
	va_start (argptr, message);
	vfprintf (stderr, message, argptr);
	va_end (argptr);
	fclose (file);
	return true;
}





int g_gl_width = 640;
int g_gl_height = 480;
GLFWwindow* g_window = NULL;

double get_time()
{
	return glfwGetTime();
}

void glfw_error_callback(int error, const char* description)
{
	fputs(description, stderr);
//	gl_log_err("%s\n", description);
}
void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
	g_gl_width = width;
	g_gl_height = height;
	printf("width %i height %i\n", width, height);
	renderer_resize_callback((float)width,(float)height);
}

static double previous_seconds; // Ugly hack by Colin - tentative until proven bug-free
void _update_fps_counter(GLFWwindow* window)
{
	previous_seconds = glfwGetTime();
	// static double previous_seconds = glfwGetTime(); Removed by Colin
	static int frame_count;
	double current_seconds = glfwGetTime();
	double elapsed_seconds = current_seconds - previous_seconds;
	if (elapsed_seconds > 0.25)
	{
		previous_seconds = current_seconds;
		double fps = (double)frame_count / elapsed_seconds;
		char tmp[128];
		sprintf(tmp, "opengl @ fps: %.2f", fps);
		glfwSetWindowTitle(window, tmp);
		frame_count = 0;
	}
	frame_count++;
}

bool start_gl()
{
	fprintf(stderr, "Starting noobEngine renderer\n");

	glfwSetErrorCallback (glfw_error_callback);
	if (!glfwInit ())
	{
		fprintf (stderr, "ERROR: could not start GLFW\n");
		return false;
	}
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	g_window = glfwCreateWindow(g_gl_width, g_gl_height, "noobForge", NULL, NULL);

	if (!g_window)
	{
		fprintf (stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}
	glfwSetWindowSizeCallback(g_window, glfw_window_size_callback);
	glfwMakeContextCurrent(g_window);

	glfwWindowHint(GLFW_SAMPLES, 4);

	renderer_init_callback();


	return true;
}


int main ()
{
//	assert(restart_gl_log());
	assert(start_gl());

	while (!glfwWindowShouldClose (g_window))
	{
		glfwPollEvents ();
		if (GLFW_PRESS == glfwGetKey (g_window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose (g_window, 1);
		}
		frame_draw_callback(0, 0, g_gl_width, g_gl_height);
		glfwSwapBuffers (g_window);
	}
	glfwTerminate();
	return 0;
}

#endif
