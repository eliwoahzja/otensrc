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
# --- C++ runtime (libc++) link setup ---------------------------------------
# AIDE's ndk-build does not load src/main/jni/Application.mk, so APP_STL :=
# c++_static is ignored and no libc++ archive ever reaches the linker. Find the
# archive in the active NDK (old sources/cxx-stl layout or the r19+ unified
# sysroot) and link it explicitly after the objects, where one-pass ld needs it.
NDK_ROOT_DERIVED := $(shell for v in '$(TARGET_LD)' '$(TARGET_CC)' '$(TARGET_CXX)' '$(TARGET_AR)' '$(TARGET_RANLIB)' '$(NDK_TOOLCHAIN_ROOT)'; do r="$${v%%/toolchains/*}"; if [ -n "$$v" ] && [ "$$r" != "$$v" ]; then echo "$$r"; break; fi; done)
NDK_ROOT_FROM_MK := $(shell for f in $(MAKEFILE_LIST); do r="$${f%%/build/core/*}"; if [ "$$r" != "$$f" ]; then echo "$$r"; break; fi; done)
NDK_ROOT_FROM_SYSROOT := $(shell for v in '$(SYSROOT)'; do r="$${v%%/platforms/*}"; if [ -n "$$v" ] && [ "$$r" != "$$v" ]; then echo "$$r"; break; fi; done)
NDK_ROOTS := $(strip $(NDK_ROOT) $(NDK_ROOT_DERIVED) $(NDK_ROOT_FROM_MK) $(NDK_ROOT_FROM_SYSROOT))
ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
NDK_ABI_TRIPLE := aarch64-linux-android
else ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
NDK_ABI_TRIPLE := arm-linux-androideabi
else ifeq ($(TARGET_ARCH_ABI),x86)
NDK_ABI_TRIPLE := i686-linux-android
else ifeq ($(TARGET_ARCH_ABI),x86_64)
NDK_ABI_TRIPLE := x86_64-linux-android
else
NDK_ABI_TRIPLE := $(TARGET_ARCH_ABI)
endif
NDK_STL_DIRS := \
  $(foreach r,$(NDK_ROOTS),$(r)/sources/cxx-stl/llvm-libc++/libs/$(TARGET_ARCH_ABI)) \
  $(foreach r,$(NDK_ROOTS),$(wildcard $(r)/sources/cxx-stl/*/libs/$(TARGET_ARCH_ABI))) \
  $(if $(strip $(NDK_UNIFIED_SYSROOT_PATH)),$(NDK_UNIFIED_SYSROOT_PATH)/usr/lib/$(NDK_ABI_TRIPLE)) \
  $(foreach r,$(NDK_ROOTS),$(wildcard $(r)/toolchains/llvm/prebuilt/*/sysroot/usr/lib/$(NDK_ABI_TRIPLE)))
NDK_CXX_STATIC := $(firstword $(wildcard $(addsuffix /libc++_static.a,$(NDK_STL_DIRS))))
NDK_CXXABI := $(firstword $(wildcard $(addsuffix /libc++abi.a,$(NDK_STL_DIRS))))
ifeq ($(strip $(NDK_CXX_STATIC)),)
$(warning Android.mk: libc++_static.a not found (NDK_ROOTS=$(NDK_ROOTS)); falling back to -lc++_static)
endif
NDK_STL_LIBDIR := $(firstword $(dir $(NDK_CXX_STATIC)))

LOCAL_LDLIBS           += -Wl,--start-group
ifneq ($(strip $(NDK_STL_LIBDIR)),)
LOCAL_LDLIBS           += -L$(NDK_STL_LIBDIR)
endif
ifneq ($(strip $(NDK_CXX_STATIC)),)
LOCAL_LDLIBS           += $(NDK_CXX_STATIC)
else
LOCAL_LDLIBS           += -lc++_static
endif
ifneq ($(strip $(NDK_CXXABI)),)
LOCAL_LDLIBS           += $(NDK_CXXABI)
endif
LOCAL_LDLIBS           += -Wl,--end-group
NDK_BUILTINS_ARCH := $(if $(filter arm64-v8a,$(TARGET_ARCH_ABI)),aarch64,$(TARGET_ARCH))
NDK_BUILTINS := $(firstword $(wildcard $(NDK_TOOLCHAIN_LIB_DIR)/libclang_rt.builtins-$(NDK_BUILTINS_ARCH)-android.a) $(foreach r,$(NDK_ROOTS),$(wildcard $(r)/toolchains/llvm/prebuilt/*/lib*/clang/*/lib/linux/libclang_rt.builtins-$(NDK_BUILTINS_ARCH)-android.a)))
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

