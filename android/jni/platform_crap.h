/* If our compiler/linker were smart enough, we would not need such things here. However, it isn't. :( */

#include <android_native_app_glue.h>
#include <android/log.h>

#include <GLES3/gl3.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity" ,__VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity" , __VA_ARGS__))

// http://www.50ply.com/blog/2013/01/19/loading-compressed-android-assets-with-file-pointer/
// Brilliant, elegant solution for apps that don't unpack to disk
// https://github.com/netguy204/gambit-game-lib/blob/dk94/android_fopen.h
#ifndef ANDROID_FOPEN_H
#define ANDROID_FOPEN_H
#include <stdio.h>
#include <android/asset_manager.h>
#ifdef __cplusplus
extern "C" {
#endif
/* hijack fopen and route it through the android asset system so that
we can pull things out of our packagesk APK */
void android_fopen_set_asset_manager(AAssetManager* manager);
FILE* android_fopen(const char* fname, const char* mode);
#define fopen(name, mode) android_fopen(name, mode)
#ifdef __cplusplus
}
#endif
#endif



