LOCAL_PATH := $(realpath $(call my-dir)/.. )

PVRSDKDIR := $(LOCAL_PATH)/libraries/PVR

include $(CLEAR_VARS)

LOCAL_MODULE    := lwfk

### Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES := 	libraries/PVR/Tools/OGLES2/PVRTgles2Ext.cpp \
					libraries/PVR/Tools/OGLES2/PVRTTextureAPI.cpp \
					libraries/PVR/Tools/OGLES2/PVRTBackground.cpp \
					libraries/PVR/Tools/OGLES2/PVRTPFXParserAPI.cpp \
					libraries/PVR/Tools/OGLES2/PVRTPFXSemantics.cpp \
					libraries/PVR/Tools/OGLES2/PVRTShader.cpp \
					libraries/PVR/Shell/PVRShell.cpp \
				   	libraries/PVR/Shell/API/KEGL/PVRShellAPI.cpp \
				   	libraries/PVR/Shell/OS/Android/PVRShellOS.cpp

LOCAL_SRC_FILES += 	libraries/PVR/Tools/PVRTFixedPoint.cpp \
					libraries/PVR/Tools/PVRTMatrixF.cpp \
					libraries/PVR/Tools/PVRTMisc.cpp \
					libraries/PVR/Tools/PVRTTrans.cpp \
					libraries/PVR/Tools/PVRTVertex.cpp \
					libraries/PVR/Tools/PVRTModelPOD.cpp \
					libraries/PVR/Tools/PVRTDecompress.cpp \
					libraries/PVR/Tools/PVRTTriStrip.cpp \
					libraries/PVR/Tools/PVRTTexture.cpp \
					libraries/PVR/Tools/PVRTResourceFile.cpp \
					libraries/PVR/Tools/PVRTString.cpp \
					libraries/PVR/Tools/PVRTStringHash.cpp \
					libraries/PVR/Tools/PVRTPFXParser.cpp \
					libraries/PVR/Tools/PVRTShadowVol.cpp \
					libraries/PVR/Tools/PVRTVector.cpp \
					libraries/PVR/Tools/PVRTError.cpp \
					libraries/PVR/Tools/PVRTUnicode.cpp \
					libraries/PVR/Tools/PVRTQuaternionF.cpp

LOCAL_SRC_FILES +=	lwfk/lwApp.cpp \
					lwfk/lwCamera.cpp \
					lwfk/lwColor.cpp \
					lwfk/lwEffects.cpp \
					lwfk/lwlog.cpp \
					lwfk/lwMaterial.cpp \
					lwfk/lwMesh.cpp \
					lwfk/lwModel.cpp \
					lwfk/lwRes.cpp \
					lwfk/lwSprite.cpp \
					lwfk/lwRenderState.cpp \
					lwfk/lwTask.cpp \
					lwfk/lwTexture.cpp \
					lwfk/lwTouch.cpp \
					lwfk/lwSoundAndroid.cpp \
					lwfk/lwText.cpp \
					lwfk/lwThread.cpp \
					libraries/soil/SOIL.c \
				   	libraries/soil/image_DXT.c \
				   	libraries/soil/image_helper.c \
				   	libraries/soil/stb_image_aug.c \
				   	libraries/tinyxml2/tinyxml2.cpp


LOCAL_C_INCLUDES := $(PVRSDKDIR)/Builds/Include \
					$(PVRSDKDIR)/Shell \
				    $(PVRSDKDIR)/Shell/API/KEGL	\
				    $(PVRSDKDIR)/Shell/OS/android \
				    $(PVRSDKDIR)/Builds/Include	\
				    $(PVRSDKDIR)/Tools \
				    $(PVRSDKDIR)/Tools/OGLES2 \
				    $(NDKDIR)/sources/cxx-stl/stlport/stlport \
				    $(LOCAL_PATH)/libraries

LOCAL_CFLAGS := -DBUILD_OGLES2

ifeq ($(TARGET_ARCH_ABI),x86)
LOCAL_CFLAGS += -fno-stack-protector 
endif

LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_STATIC_LIBRARY)

