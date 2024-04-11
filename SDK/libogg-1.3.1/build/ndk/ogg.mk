
# ogg NDK makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := ogg

OGG_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/libogg-1.3.1)
OGG_INCLUDE := $(SDK_INC_ROOT)/libogg-1.3.1/include 

OGG_SRC := $(OGG_FOLDER)/src/bitwise.c \
$(OGG_FOLDER)/src/framing.c

LOCAL_C_INCLUDES := $(OGG_INCLUDE)
LOCAL_SRC_FILES :=  $(OGG_SRC)
LOCAL_CFLAGS 	:= -ffast-math -O3 -funroll-loops
LOCAL_CFLAGS += -DLIBOGG_EXPORTS

include $(BUILD_STATIC_LIBRARY)
