/* If our compiler/linker were smart enough, we would not need such things here. However, it isn't. :( */

#include <android_native_app_glue.h>
#include <android/log.h>
#include <GLES3/gl3.h>

#define LOG(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity" ,__VA_ARGS__))
 

