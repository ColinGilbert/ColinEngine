LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_ARM_MODE	 := arm
LOCAL_C_INCLUDES :=$(LOCAL_PATH)/engine \
	$(LOCAL_PATH)/engine/fs \
	$(LOCAL_PATH)/engine/core \
	$(LOCAL_PATH)/engine/sound \
	$(LOCAL_PATH)/engine/threading

LOCAL_MODULE     := native-activity
LOCAL_SRC_FILES  := main.cpp
LOCAL_LDLIBS     := -llog -landroid -lEGL -lGLESv3
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
