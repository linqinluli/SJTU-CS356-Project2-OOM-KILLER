LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := oom.c   # your source code
LOCAL_MODULE := oom    # output file name
LOCAL_CFLAGS += -pie -fPIE   # These two line mustn��t be
LOCAL_LDFLAGS += -pie -fPIE  # change.
LOCAL_FORCE_STATIC_EXECUTABLE := true

export KBUILD_EXTRA_SYMBOLS
KBUILD_EXTRA_SYMBOLS += /home/linqinluli/osprj2/module1/Module.symvers


include $(BUILD_EXECUTABLE)
