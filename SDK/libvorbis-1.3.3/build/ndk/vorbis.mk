
# vorbis NDK makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := vorbis

OGG_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/libogg-1.3.1)
OGG_INCLUDE := $(SDK_INC_ROOT)/libogg-1.3.1/include 

VORBIS_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/libvorbis-1.3.3)
VORBIS_INCLUDE := $(SDK_INC_ROOT)/libvorbis-1.3.3/include $(OGG_INCLUDE)

VORBIS_SRC := $(VORBIS_FOLDER)/lib/analysis.c \
$(VORBIS_FOLDER)/lib/bitrate.c \
$(VORBIS_FOLDER)/lib/block.c \
$(VORBIS_FOLDER)/lib/codebook.c \
$(VORBIS_FOLDER)/lib/envelope.c \
$(VORBIS_FOLDER)/lib/floor0.c \
$(VORBIS_FOLDER)/lib/floor1.c \
$(VORBIS_FOLDER)/lib/info.c \
$(VORBIS_FOLDER)/lib/lookup.c \
$(VORBIS_FOLDER)/lib/lpc.c \
$(VORBIS_FOLDER)/lib/lsp.c \
$(VORBIS_FOLDER)/lib/mapping0.c \
$(VORBIS_FOLDER)/lib/mdct.c \
$(VORBIS_FOLDER)/lib/psy.c \
$(VORBIS_FOLDER)/lib/registry.c \
$(VORBIS_FOLDER)/lib/res0.c \
$(VORBIS_FOLDER)/lib/sharedbook.c \
$(VORBIS_FOLDER)/lib/smallft.c \
$(VORBIS_FOLDER)/lib/synthesis.c \
$(VORBIS_FOLDER)/lib/window.c \
$(VORBIS_FOLDER)/lib/vorbisfile.c

LOCAL_C_INCLUDES := $(VORBIS_INCLUDE)
LOCAL_SRC_FILES :=  $(VORBIS_SRC)

LOCAL_CFLAGS 	:= -ffast-math -O3 -funroll-loops
LOCAL_CFLAGS 	+= -DLIBVORBIS_EXPORTS -DLIBVORBISFILE_EXPORTS

include $(BUILD_STATIC_LIBRARY)
