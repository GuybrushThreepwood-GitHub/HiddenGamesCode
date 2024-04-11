
# tinyxml2 NDK makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := tinyxml2

TINYXML2_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/tinyxml2)
TINYXML2_INCLUDE := $(SDK_INC_ROOT)/tinyxml2/include

TINYXML2_SRC := $(TINYXML2_FOLDER)/src/tinyxml2.cpp

LOCAL_C_INCLUDES := $(TINYXML2_INCLUDE)
LOCAL_SRC_FILES :=  $(TINYXML2_SRC)

LOCAL_CFLAGS 	:= -ffast-math -O3 -funroll-loops

include $(BUILD_STATIC_LIBRARY)
