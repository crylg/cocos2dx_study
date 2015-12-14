LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := vDDZComm
LOCAL_MODULE_FILENAME := libvDDZComm

SRC_FILES := $(wildcard $(LOCAL_PATH)/Src/*.cpp)
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(SRC_FILES))


LOCAL_C_INCLUDES := $(LOCAL_PATH)/Inc\
 $(LOCAL_PATH)/Src\
 $(DEV_ROOT)/Framework/Core/Inc

include $(BUILD_STATIC_LIBRARY)
