#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/asset_manager.h>



static AAsset* open_file ( void *AAssetManager, const char *file_name )
{
	AAsset *pFile = NULL;

	if ( AAssetManager != NULL )
	{
		AAssetManager *asset_manager = ( AAssetManager * ) AAssetManager;
		pFile = AAssetManager_open ( asset_manager, file_name, AASSET_MODE_BUFFER );
	}

	return pFile;
}

static void close_file ( AAsset *pFile )
{
	if ( pFile != NULL )
	{

		AAsset_close ( pFile );

	}
}

static int read_file ( AAsset *pFile, int bytesToRead, void * buffer )
{
	int bytes_read = 0;
	if ( pFile == NULL )
	{
		return bytes_read;
	}

	bytes_read = AAsset_read ( pFile, buffer, bytesToRead );

	return bytes_read;
}

char * load_tga ( void * AAssetManager, const char * file_name, int * width, int * height )
{
	char *buffer;
	AAsset *fp;
	TGA_HEADER header;
	int bytes_read;
	// Open the file for reading
	fp = open_file( AAssetManager, file_name );
	if ( fp == NULL )
	{
		return NULL;
	}
	bytes_read = read_file ( fp, sizeof ( TGA_HEADER ), &header );
	*width = header.Width;
	*height = header.Height;
	if ( header.ColorDepth == 8 ||
			header.ColorDepth == 24 || header.ColorDepth == 32 )
	{
		int bytesToRead = sizeof ( char ) * ( *width ) * ( *height ) * header.ColorDepth / 8;
		// Allocate the image data buffer
		buffer = ( char * ) malloc ( bytesToRead );
		if ( buffer )
		{
			bytes_read = read_file ( fp, bytesToRead, buffer );
			close_file ( fp );
			return ( buffer );
		}
	}
	return ( NULL );
}
