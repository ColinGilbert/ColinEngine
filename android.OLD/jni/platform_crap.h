/* If our compiler/linker were smart enough, we would not need such things here. However, it isn't. :( */

#include <android_native_app_glue.h>
#include <android/log.h>
#include <stdio.h>
#include <android/asset_manager.h>
#include <GLES3/gl3.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity" ,__VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity" , __VA_ARGS__))
#define LOG(...) LOGI(...)
// http://www.50ply.com/blog/2013/01/19/loading-compressed-android-assets-with-file-pointer/
// Elegant solution for apps that don't unpack to disk
// https://github.com/netguy204/gambit-game-lib/blob/dk94/android_fopen.h

//#ifdef __cplusplus
//extern "C" {
//#endif 
//	/* Hijack fopen and route it through the android asset system so that we can pull things out of our packages APK */
//	void android_fopen_set_asset_manager(AAssetManager* manager);
//	FILE* android_fopen(const char* fname, const char* mode);
//#define fopen(name, mode) android_fopen(name, mode)
//#ifdef __cplusplus
//}
//#endif

#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "file.h"

AAssetManager* g_pManager = NULL;
void SetAssetManager( AAssetManager* pManager )
{
	g_pManager = pManager;
}

// Read the contents of the give file return the content and the file size.
// The calling function is responsible for free the memory allocated for Content.
void read_file_inside_apk_to_memory(const char* pFileName, char** ppContent, unsigned int* pSize)
{
	assert(g_pManager);
	AAsset* pFile = AAssetManager_open(g_pManager, pFileName, AASSET_MODE_UNKNOWN);
	if( pFile != nullptr )
	{
		// Determine file size
		off_t fileSize = AAsset_getLength(pFile);
		// Read data
		char* pData = (char*)malloc(fileSize);
		AAsset_read( pFile, pData, fileSize );
		// Allocate space for the file content
		*ppContent = (char*)malloc(fileSize);
		// Copy the content
		memcpy(*ppContent, pData, fileSize);
		*pSize = fileSize;
		free(pData);
		// Close the file
		AAsset_close(pFile);
	}
}

// From https://github.com/Cristianohh/ETC2TextureLoader/blob/master/jni/texture.c

GLuint load_texture( const char* TextureFileName )
{
	// Read/Load Texture File
	char* pData = NULL;
	unsigned int fileSize = 0;
	read_file( TextureFileName, &pData, &fileSize );
	// Generate handle & Load Texture
	GLuint handle = 0;
	GLenum target;
	GLboolean mipmapped;
	KTX_error_code result = ktxload_textureM( pData, fileSize, &handle, &target, NULL, &mipmapped, NULL, NULL, NULL );
	if( result != KTX_SUCCESS )
	{
		LogError( "KTXLib couldn't load texture %s. Error: %d", TextureFileName, result );
		return 0;
	}
	// Bind the texture
	glBindTexture( target, handle );
	// Set filtering mode for 2D textures (bilinear filtering)
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	if( mipmapped )
	{
		// Use mipmaps with bilinear filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
	}
	// clean up
	free( pData );
	// Return handle
	return handle;
}

