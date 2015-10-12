LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := layout
LOCAL_SRC_FILES := layout.c dalvik_base.c
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog


include $(BUILD_SHARED_LIBRARY)