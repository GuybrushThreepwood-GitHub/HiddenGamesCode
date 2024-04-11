
# vorbis Raspberry Pi makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := vorbis

VORBIS_FOLDER := ../..
VORBIS_INCLUDE := ../../include -I../../../libogg-1.3.1/include

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

include $(BUILD_STATIC_LIBRARY)

#################

OBJDIR := obj
ROOT_CODE_FOLDER := $(VORBIS_FOLDER)
LOCAL_CFLAGS += -march=armv6 -mfpu=vfp -mfloat-abi=hard
LOCAL_CFLAGS 	+= -DLIBVORBIS_EXPORTS -DLIBVORBISFILE_EXPORTS

FILTERED_OBJECTS := $(patsubst $(ROOT_CODE_FOLDER)/%, $(OBJDIR)/%, $(LOCAL_SRC_FILES) ) 
C_OBJECTS := $(FILTERED_OBJECTS:.c=.o)
OBJECTS := $(C_OBJECTS:.cpp=.o)
OUTFOLDERS := $(dir $(OBJECTS) ) 

LIBRARY := ../../libs/lib$(LOCAL_MODULE).a
MKDIR := mkdir

all: $(OBJDIR) $(LIBRARY)
		
$(LIBRARY): $(OBJECTS) 
	ar rcs $(LIBRARY) $(OBJECTS)
		
$(OBJDIR): 
	$(MKDIR) -p $@
	$(MKDIR) -p $(OUTFOLDERS)
	
$(OBJDIR)/%.o : $(ROOT_CODE_FOLDER)/%.c
	gcc $(LOCAL_CFLAGS) -I$(LOCAL_C_INCLUDES) -c $< -o $@
	
$(OBJDIR)/%.o : $(ROOT_CODE_FOLDER)/%.cpp
	gcc $(LOCAL_CFLAGS) -I$(LOCAL_C_INCLUDES) -c $< -o $@
	
clean:
	$(RM) $(LIBRARY)
	$(RM) -rf $(OBJDIR)
	
