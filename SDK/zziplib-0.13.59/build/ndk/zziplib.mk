
# zziplib NDK makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := zziplib

ZZIPLIB_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/zziplib-0.13.59)
ZZIPLIB_INCLUDE := $(SDK_INC_ROOT)/zziplib-0.13.59/include

ZZIPLIB_SRC := $(ZZIPLIB_FOLDER)/src/zzip/dir.c \
$(ZZIPLIB_FOLDER)/src/zzip/err.c \
$(ZZIPLIB_FOLDER)/src/zzip/fetch.c \
$(ZZIPLIB_FOLDER)/src/zzip/file.c \
$(ZZIPLIB_FOLDER)/src/zzip/fseeko.c \
$(ZZIPLIB_FOLDER)/src/zzip/info.c \
$(ZZIPLIB_FOLDER)/src/zzip/memdisk.c \
$(ZZIPLIB_FOLDER)/src/zzip/mmapped.c \
$(ZZIPLIB_FOLDER)/src/zzip/plugin.c \
$(ZZIPLIB_FOLDER)/src/zzip/stat.c \
$(ZZIPLIB_FOLDER)/src/zzip/write.c \
$(ZZIPLIB_FOLDER)/src/zzip/zip.c

LOCAL_C_INCLUDES := $(ZZIPLIB_INCLUDE)
LOCAL_SRC_FILES :=  $(ZZIPLIB_SRC)

LOCAL_CFLAGS 	:= -ffast-math -O3 -funroll-loops

include $(BUILD_STATIC_LIBRARY)
