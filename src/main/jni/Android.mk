LOCAL_PATH := $(call my-dir)
MAIN_LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libcurl
LOCAL_SRC_FILES := curl/curl-android-$(TARGET_ARCH_ABI)/lib/libcurl.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libssl
LOCAL_SRC_FILES := curl/openssl-android-$(TARGET_ARCH_ABI)/lib/libssl.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libcrypto
LOCAL_SRC_FILES := curl/openssl-android-$(TARGET_ARCH_ABI)/lib/libcrypto.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := libdobby
LOCAL_SRC_FILES         := IL2CppSDKGenerator/Dobby/libraries/$(TARGET_ARCH_ABI)/libdobby.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/IL2CppSDKGenerator/Dobby/
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libfoxcheats
LOCAL_SRC_FILES := foxcheats/libs/$(TARGET_ARCH_ABI)/libfoxcheats.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libctorHook
LOCAL_SRC_FILES := ctorHook/libs/$(TARGET_ARCH_ABI)/libctorHook.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

include $(CLEAR_VARS)
LOCAL_MODULE := libxhook
LOCAL_SRC_FILES := \
    SDK/Xhook/xhook.c \
    SDK/Xhook/xh_core.c \
    SDK/Xhook/xh_jni.c \
    SDK/Xhook/xh_elf.c \
    SDK/Xhook/xh_log.c \
    SDK/Xhook/xh_util.c \
    SDK/Xhook/xh_version.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/SDK/Xhook
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/SDK/Xhook
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE           := libV2

ifeq ($(LOCAL_MODULE),libV2)
  $(shell rm -f $(LOCAL_PATH)/libs/$(TARGET_ARCH_ABI)/libv2.so)
endif
LOCAL_CFLAGS           := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w
LOCAL_CFLAGS           += -fno-rtti -fno-exceptions -fpermissive
LOCAL_CPPFLAGS         := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w -Werror -s -std=c++17 -DETHNIR_LIQUID_SHADER
LOCAL_CPPFLAGS         += -Wno-error=c++11-narrowing -fms-extensions -fno-rtti -fno-exceptions -fpermissive

LOCAL_LDFLAGS          += -Wl,--gc-sections,--strip-all, -llog
LOCAL_ARM_MODE         := arm
LOCAL_LDLIBS           := -llog -landroid -lEGL -lGLESv3 -lGLESv2 -lGLESv1_CM -lz

LOCAL_C_INCLUDES       += $(LOCAL_PATH)
FILE_LIST              := $(wildcard $(LOCAL_PATH)/ImGui/*.c*)
FILE_LIST              += $(wildcard $(LOCAL_PATH)/IL2CppSDKGenerator/IL2Cpp/*.c*)
FILE_LIST              += $(wildcard $(LOCAL_PATH)/IL2CppSDKGenerator/KittyMemory/*.c*)
FILE_LIST              += $(wildcard $(LOCAL_PATH)/System/Texture/*.c*)
FILE_LIST              += $(wildcard $(LOCAL_PATH)/*.c*)

LOCAL_SRC_FILES        := $(FILE_LIST:$(LOCAL_PATH)/%=%) \
                              Substrate/hde64.c \
                              Substrate/SubstrateDebug.cpp \
                              Substrate/SubstrateHook.cpp \
                              Substrate/SubstratePosixMemory.cpp \
                              Substrate/And64InlineHook.cpp \
                              AstralPrtctn/md5.cpp

LOCAL_LDLIBS := -llog -landroid
FILE_LIST := $(wildcard $(LOCAL_PATH)/libzip/*.c)
LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/curl/curl-android-$(TARGET_ARCH_ABI)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/curl/openssl-android-$(TARGET_ARCH_ABI)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/libzip

LOCAL_LDLIBS           := -llog -landroid -lz -lEGL -lGLESv2 -lGLESv3
LOCAL_LDLIBS           += -static-libstdc++ -Wl,--start-group
NDK_CXX_STATIC := $(firstword $(wildcard $(NDK_UNIFIED_SYSROOT_PATH)/usr/lib/*/libc++_static.a) $(wildcard $(NDK_UNIFIED_SYSROOT_PATH)/usr/lib/*/*/libc++_static.a) $(wildcard $(NDK_ROOT)/toolchains/llvm/prebuilt/*/sysroot/usr/lib/*/libc++_static.a) $(wildcard $(NDK_ROOT)/toolchains/llvm/prebuilt/*/sysroot/usr/lib/*/*/libc++_static.a))
ifneq ($(strip $(NDK_CXX_STATIC)),)
LOCAL_LDLIBS           += $(NDK_CXX_STATIC)
else
LOCAL_LDLIBS           += -lc++_static
endif
NDK_CXXABI := $(firstword $(wildcard $(NDK_UNIFIED_SYSROOT_PATH)/usr/lib/*/libc++abi.a) $(wildcard $(NDK_UNIFIED_SYSROOT_PATH)/usr/lib/*/*/libc++abi.a) $(wildcard $(NDK_ROOT)/toolchains/llvm/prebuilt/*/sysroot/usr/lib/*/libc++abi.a) $(wildcard $(NDK_ROOT)/toolchains/llvm/prebuilt/*/sysroot/usr/lib/*/*/libc++abi.a))
ifneq ($(strip $(NDK_CXXABI)),)
LOCAL_LDLIBS           += $(NDK_CXXABI)
endif
LOCAL_LDLIBS           += -Wl,--end-group
NDK_BUILTINS_ARCH := $(if $(filter arm64-v8a,$(TARGET_ARCH_ABI)),aarch64,$(TARGET_ARCH))
NDK_BUILTINS := $(firstword $(wildcard $(NDK_TOOLCHAIN_LIB_DIR)/libclang_rt.builtins-$(NDK_BUILTINS_ARCH)-android.a) $(wildcard $(NDK_ROOT)/toolchains/llvm/prebuilt/*/lib*/clang/*/lib/linux/libclang_rt.builtins-$(NDK_BUILTINS_ARCH)-android.a))
ifneq ($(strip $(NDK_BUILTINS)),)
LOCAL_LDLIBS           += $(NDK_BUILTINS)
endif
LOCAL_STATIC_LIBRARIES := libcurl libssl libcrypto libdobby libfoxcheats libxhook libctorHook

LOCAL_CPP_FEATURES                      := exceptions
LOCAL_C_INCLUDES := $(LOCAL_PATH)/curl/curl-android-$(TARGET_ARCH_ABI)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/curl/openssl-android-$(TARGET_ARCH_ABI)/include

LOCAL_C_INCLUDES += $(LOCAL_PATH)/foxcheats
LOCAL_C_INCLUDES += $(LOCAL_PATH)/foxcheats/includes

include $(BUILD_SHARED_LIBRARY)

