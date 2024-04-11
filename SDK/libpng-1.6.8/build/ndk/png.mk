
# png NDK makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := png

PNG_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/libpng-1.6.8)
PNG_INCLUDE := $(SDK_INC_ROOT)/libpng-1.6.8/include

PNG_SRC := $(PNG_FOLDER)/src/png.c \
$(PNG_FOLDER)/src/pngerror.c \
$(PNG_FOLDER)/src/pngget.c \
$(PNG_FOLDER)/src/pngmem.c \
$(PNG_FOLDER)/src/pngpread.c \
$(PNG_FOLDER)/src/pngread.c \
$(PNG_FOLDER)/src/pngrio.c \
$(PNG_FOLDER)/src/pngrtran.c \
$(PNG_FOLDER)/src/pngrutil.c \
$(PNG_FOLDER)/src/pngset.c \
$(PNG_FOLDER)/src/pngtrans.c \
$(PNG_FOLDER)/src/pngwio.c \
$(PNG_FOLDER)/src/pngwrite.c \
$(PNG_FOLDER)/src/pngwtran.c \
$(PNG_FOLDER)/src/pngwutil.c

LOCAL_C_INCLUDES := $(PNG_INCLUDE)
LOCAL_SRC_FILES :=  $(PNG_SRC)

LOCAL_CFLAGS 	:= -ffast-math -O3 -funroll-loops

include $(BUILD_STATIC_LIBRARY)
