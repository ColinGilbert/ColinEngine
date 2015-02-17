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

#include <stdio.h>

#include "nanovg.h"
#define NANOVG_GLES3_IMPLEMENTATION
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"


NVGcontext* vg = NULL;

class Texture
{
	public:
		Texture(GLuint _handle, unsigned short _height, unsigned short _width , unsigned short _mips)
		{
			handle = _handle;
			height = _height;
			width = _width;
			mips = _mips;
		}

		~Texture() = default;
		GLuint get_handle() { return handle; }
		unsigned short get_height() { return height; }
		unsigned short get_width() { return width; }

	protected:
		GLuint handle;
		unsigned short height,width,mips;
};


typedef struct
{
	unsigned char IdSize,
		      MapType,
		      ImageType;
	unsigned short PaletteStart,
		       PaletteSize;
	unsigned char PaletteEntryDepth;
	unsigned short X,
		       Y,
		       Width,
		       Height;
	unsigned char ColorDepth,
		      Descriptor;
} TGA_HEADER;



/* --------------- Externs -------------- */

extern char * load_tga(const char * name, unsigned short* width, unsigned short* height);

extern double get_time();

extern void exit_app();

/* ---------- Callbacks ---------- */



void app_init_callback()
{
}
void app_save_state_callback() {}
void app_exit_callback() {}
void renderer_init_callback()
{
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
//	LOG("Renderer: %s\n", renderer);
//	LOG("OpenGL version supported %s\n", version);
//
	unsigned short w, h;
	char * tex_data = load_tga("romeo.tga",&w,&h);

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
