
# zlib NDK makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := zlib

ZLIB_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/zlib-1.2.8)
ZLIB_INCLUDE := $(SDK_INC_ROOT)/zlib-1.2.8/include

ZLIB_SRC := $(ZLIB_FOLDER)/src/adler32.c \
$(ZLIB_FOLDER)/src/compress.c \
$(ZLIB_FOLDER)/src/crc32.c \
$(ZLIB_FOLDER)/src/deflate.c \
$(ZLIB_FOLDER)/src/gzclose.c \
$(ZLIB_FOLDER)/src/gzlib.c \
$(ZLIB_FOLDER)/src/gzread.c \
$(ZLIB_FOLDER)/src/gzwrite.c \
$(ZLIB_FOLDER)/src/infback.c \
$(ZLIB_FOLDER)/src/inffast.c \
$(ZLIB_FOLDER)/src/inflate.c \
$(ZLIB_FOLDER)/src/inftrees.c \
$(ZLIB_FOLDER)/src/trees.c \
$(ZLIB_FOLDER)/src/uncompr.c \
$(ZLIB_FOLDER)/src/zutil.c

LOCAL_C_INCLUDES := $(ZLIB_INCLUDE)
LOCAL_SRC_FILES :=  $(ZLIB_SRC)

LOCAL_CFLAGS 	:= -ffast-math -O3 -funroll-loops

include $(BUILD_STATIC_LIBRARY)
