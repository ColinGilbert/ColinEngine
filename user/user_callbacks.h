/*
 * This file is glue. Its API has just begun forming. Function signatures WILL change. If you are adventurous enough to start using this code as-is, you'll probably need to fix broken code in the future.
 *
 * The reasoning behind this API is as follows:
 * 
 * Functions calling directly into the target platform are marked 'extern'.
 * They are a thin wrapper of abstraction over OS-level code and once they're implemented at the target OS level, end-user should never need to modify them.
 *
 * Functions marked with the suffix '_callback' with the pretty curly braces stubs are yours to implement.
 * They hook into calls from the target platform; these functions are basically signals that events have ocurred at the UI and OS levels.
 *
 */
#include <memory>
#include <list>
//#include "stb_image.h"


/* ---------- Externs/call-forwards ----------- */

extern double get_time();
extern void exit_app();
extern GLuint load_texture(const std::string& name); 

/* ---------- Callbacks ---------- */

void app_init_callback() {}
void app_save_state_callback() {}
void app_exit_callback() {}
void renderer_init_callback()
{
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
//	LOG("Renderer: %s\n", renderer);
//	LOG("OpenGL version supported %s\n", version);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

void renderer_start_callback() {}
void renderer_stop_callback() {}
void renderer_resize_callback(float width, float height) {}
void renderer_destroy_callback() {}
void frame_draw_callback(float width, float height)
{
	glClearColor(0.4, 0.1, 0.4, 1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}
void accelerometer_enable_callback() {}
void accelerometer_disable_callback() {}
void accelerometer_input_callback(float x, float y, float z) {}
void key_event_callback(int code, int state) {}
void cursor_event_callback(float x, float y, short event_type) {}
