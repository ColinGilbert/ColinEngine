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
#include "stb_truetype.h"

GLuint load_shader(GLenum type, const char *shader_src)
{
	GLuint shader;
	GLint compiled;
	// Create the shader object
	shader = glCreateShader(type);
	if (shader == 0)
	{
		return 0;
	}
	glShaderSource(shader, 1, &shader_src, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLint info_len = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
		if (info_len > 1)
		{
			char *info_log = malloc(sizeof(char) * info_len);
			glGetShaderInfoLog(shader, info_len, NULL, info_log);
			LOG("Error compiling shader:\n%s\n", info_log);
			free(info_log);
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

GLuint load_shader_program(const char* vert_shader_src, const char* frag_shader_src)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program_object;
	GLint linked;
	vertex_shader = esLoadShader(GL_VERTEX_SHADER, vert_shader_src);
	if ( vertex_shader == 0 )
	{
		return 0;
	}
	fragment_shader = esLoadShader(GL_FRAGMENT_SHADER, frag_shader_src);
	if (fragment_shader == 0)
	{
		glDeleteShader(vertex_shader);
		return 0;
	}
	program_object = glCreateProgram();
	if (program_object == 0)
	{
		return 0;
	}
	glAttachShader(program_object, vertex_shader);
	glAttachShader(program_object, fragment_shader);
	glLinkProgram(program_object);
	glGetProgramiv(program_object, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLint info_len = 0;
		glGetProgramiv(program_object, GL_INFO_LOG_LENGTH, &info_len);
		if (info_len > 1)
		{
			char *info_log = malloc(sizeof(char) * info_len);
			glGetProgramInfoLog(program_object, info_len, NULL, info_log);
			LOG("Error linking program:\n%s\n", info_log);
			free(info_log);
		}
		glDeleteProgram(program_object);
		return 0;
	}
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	return program_object;
}

char text_vertex_shader[] =
"#version 300 es                                                                        \n" 
"uniform mediump mat4 MODELVIEWPROJECTIONMATRIX;                                        \n"
"attribute mediump vec2 POSITION;                                                       \n"
"attribute lowp vec2 TEXCOORD0;                                                         \n"
"out lowp vec2 texcoord0;                                                               \n"
"void main() {                                                                          \n" 
"texcoord0 = TEXCOORD0;                                                                 \n"
"gl_Position = MODELVIEWPROJECTIONMATRIX * vec4( POSITION.x, POSITION.y, 0.0, 1.0 ); }  \n";

char text_fragment_shader[] =
"#version 300 es                                      \n"
"uniform sampler2D DIFFUSE;                           \n"
"uniform lowp vec4 COLOR;                             \n"
"in lowp vec2 texcoord0;                              \n"
"void main() {                                        \n"
"lowp vec4 color = texture2D( DIFFUSE, texcoord0 );   \n"
"color.x = COLOR.x;                                   \n"
"color.y = COLOR.y;                                   \n"
"color.z = COLOR.z;                                   \n"
"color.w *= COLOR.w;                                  \n"
"gl_FragColor = color; }                              \n";

typedef struct
{
	char name[256];
	stbtt_bakedchar* character_data;
	float font_size;
	int texture_width;
	int texture_height;
	int first_character;
	int count_character;
	GLuint program;
	unsigned int tid;
} font;

font* font_init(char* name)
{
	font* _font = (font *) calloc(1, sizeof(font));
	strcpy(font->name, name);

	return font;

}

font* font_free(font* _font)
{
	if(font->program)
	{
		SHADER_free(font->program->vertex_shader);
		SHADER_free(font->program->fragment_shader);
		PROGRAM_free(font->program);
	}
	if (_font->character_data) free(_font->character_data);
	if (_font->tid) glDeleteTextures(1, &font->tid);
	free(_font);
	return NULL;
}

unsigned char font_load(font* _font , char* filename, unsigned char relative_path, float size, unsigned int texture_width, unsigned int texture_height, int first_character, int count_character)
{
	MEMORY *m = mopen(filename, relative_path);
	if(m)
	{
		unsigned char* texel_array = (unsigned char*) malloc(texture_width * texture_height);
		font->character_data = (stbtt_bakedchar*) malloc(count_character * sizeof(stbtt_bakedchar));
		font->size = font_size;
		font->texture_width = texture_width;
		font->texture_height = texture_height;
		font->first_character = first_character;
		font->count_character = count_character;
		stbtt_BakeFontBitmap( m->buffer,0,size,texel_array,texture_width,texture_height,first_character,count_character,font->character_data);
		mclose(m);
		glGenTextures(1, &font->tid);
		glBindTexture(GL_TEXTURE_2D, font->tid);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, texture_width, texture_height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, texel_array);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		free(texel_array);
	}
	return 0;
}

void font_print( font* _font, float x, float y, char* text, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	char vertex_attribute = PROGRAM_get_vertex_attrib_location(_font->program, (char*)"POSITION");
	char texcoord_attribute = PROGRAM_get_vertex_attrib_location(_font->program, (char*)"TEXCOORD0");
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	PROGRAM_draw(_font->program);
	glUniformMatrix4fv( PROGRAM_get_uniform_location(_font->program, (char*)"MODELVIEWPROJECTIONMATRIX"), 1, GL_FALSE, (float*)GFX_get_modelview_projection_matrix());
	glUniform1i(PROGRAM_get_uniform_location(_font->program, (char*)"DIFFUSE"), 0);
	if(color)
	{
		glUniform4fv(PROGRAM_get_uniform_location(_font->program, (char*)"COLOR"), 1, (float*)color);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _font->tid);
	glEnableVertexAttribArray(vertex_attribute);
	glEnableVertexAttribArray(texcoord_attribute);
	while(*text)
	{
		if(*text >= _font->first_character && *text <= (_font->first_character + _font->count_character))
		{
			vec2 vert[4];
			vec2 uv[4];
			stbtt_aligned_quad quad;
			stbtt_bakedchar *bakedchar = _font->character_data + (*text - _font->first_character);
			int round_x = STBTT_ifloor(x + bakedchar->xoff);
			int round_y = STBTT_ifloor(y - bakedchar->yoff);
			quad.x0 = (float)round_x;
			quad.y0 = (float)round_y;
			quad.x1 = (float)round_x + bakedchar->x1 - bakedchar->x0;
			quad.y1 = (float)round_y - bakedchar->y1 + bakedchar->y0;
			quad.s0 = bakedchar->x0 / (float)_font->texture_width;
			quad.t0 = bakedchar->y0 / (float)_font->texture_width;
			quad.s1 = bakedchar->x1 / (float)_font->texture_height;
			quad.t1 = bakedchar->y1 / (float)_font->texture_height;
			x += bakedchar->xadvance;
			vert[0].x = quad.x1; vert[0].y = quad.y0;
			uv[0].x = quad.s1; uv[0].y = quad.t0;
			vert[1].x = quad.x0; vert[1].y = quad.y0;
			uv[1].x = quad.s0; uv[1].y = quad.t0;
			vert[2].x = quad.x1; vert[2 ].y = quad.y1;
			uv[2].x = quad.s1; uv[2].y = quad.t1;
			vert[3].x = quad.x0; vert[3].y = quad.y1;
			uv[3].x = quad.s0; uv[3].y = quad.t1;
			glVertexAttribPointer(vertex_attribute, 2, GL_FLOAT, GL_FALSE, 0, (float*)&vert[0]);
			glVertexAttribPointer(texcoord_attribute, 2, GL_FLOAT, GL_FALSE, 0,(float*)&uv[0]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
		++text;
	}
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}	

float font_length(font* _font, char* text )
{
	float length = 0;
	while(*text)
	{
		if(*text >= _font->first_character && *text <= (_font->first_character + _font->count_character))
		{
			stbtt_bakedchar *bakedchar = _font->character_data + ( *text - _font->first_character );
			length += bakedchar->xadvance;
		}
		++text;
	}
	return length;
}

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

// Loads file from disk/apk into mem. Returns bytes read.
int load_file_into_mem(const char* file_name, void* data);

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
	char* tex_data = load_tga("romeo.tga",&w,&h);

	glEnable(GL_STENCIL_TEST);
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
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);


}
void accelerometer_enable_callback() {}
void accelerometer_disable_callback() {}
void accelerometer_input_callback(float x, float y, float z) {}
void key_event_callback(int code, int state) {}
void cursor_event_callback(float x, float y, short event_type) {}
