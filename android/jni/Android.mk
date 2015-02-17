LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := native-activity

LOCAL_ARM_MODE	 := arm

LOCAL_C_FLAGS := -Werror

LOCAL_C_INCLUDES :=$(LOCAL_PATH)/engine \
	$(LOCAL_PATH)/engine/fs \
	$(LOCAL_PATH)/engine/core \
	$(LOCAL_PATH)/engine/sound \
	$(LOCAL_PATH)/engine/threading \
	$(LOCAL_PATH)/engine/extern/nanovg/src


LOCAL_SRC_FILES  := main.cpp \
	engine/extern/nanovg/src/nanovg.c

#\
	engine/extern/KTX/checkheader.c \
	engine/extern/KTX/hashtable.c  \
	engine/extern/KTX/loader.c \
	engine/extern/KTX/swap.c  \
	engine/extern/KTX/writer.c \
	engine/extern/KTX/etcunpack.cxx\
	engine/extern/KTX/etcdec.cxx \
	engine/extern/KTX/errstr.c \

	

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv3
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
