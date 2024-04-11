
# jsonbox NDK makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := jsonbox

JSONBOX_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/jsonbox-0.4.4)
JSONBOX_INCLUDE := $(SDK_INC_ROOT)/jsonbox-0.4.4/include

JSONBOX_SRC := $(JSONBOX_FOLDER)/src/Array.cpp \
$(JSONBOX_FOLDER)/src/Convert.cpp \
$(JSONBOX_FOLDER)/src/Escaper.cpp \
$(JSONBOX_FOLDER)/src/IndentCanceller.cpp \
$(JSONBOX_FOLDER)/src/Indenter.cpp \
$(JSONBOX_FOLDER)/src/Object.cpp \
$(JSONBOX_FOLDER)/src/SolidusEscaper.cpp \
$(JSONBOX_FOLDER)/src/Value.cpp

LOCAL_CFLAGS := -ffast-math -O3 -funroll-loops

LOCAL_C_INCLUDES := $(JSONBOX_INCLUDE)
LOCAL_SRC_FILES :=  $(JSONBOX_SRC)

include $(BUILD_STATIC_LIBRARY)
