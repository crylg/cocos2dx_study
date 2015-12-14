LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := vGFXStatic
LOCAL_MODULE_FILENAME := libvGFX

SRC_FILES := $(wildcard $(LOCAL_PATH)/Src/*.cpp)
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(SRC_FILES))

LOCAL_C_INCLUDES := $(LOCAL_PATH)/Inc\
	$(LOCAL_PATH)/Src\
  $(DEV_ROOT)/Framework/Core/Inc\
  $(DEV_ROOT)/Client/cocos2d\
	$(DEV_ROOT)/Client/cocos2d/cocos/editor-support\
	$(DEV_ROOT)/Client/cocos2d/cocos\
	$(DEV_ROOT)/Client/cocos2d/cocos/platform\
	$(DEV_ROOT)/Client/cocos2d/cocos/platform/desktop\
	$(DEV_ROOT)/Client/cocos2d/external


include $(BUILD_STATIC_LIBRARY)


$(call import-add-path,$(DEV_ROOT)/Client/cocos2d)
$(call import-add-path,$(DEV_ROOT)/Client/cocos2d/external)
$(call import-add-path,$(DEV_ROOT)/Client/cocos2d/cocos)

$(call import-module,.)