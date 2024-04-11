
# freetype Raspberry Pi makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := freetype

FREETYPE_FOLDER := ../..
FREETYPE_INCLUDE := ../../include

FREETYPE_SRC := $(FREETYPE_FOLDER)/src/autofit/autofit.c \
$(FREETYPE_FOLDER)/src/bdf/bdf.c \
$(FREETYPE_FOLDER)/src/cff/cff.c \
$(FREETYPE_FOLDER)/src/base/ftbase.c \
$(FREETYPE_FOLDER)/src/base/ftbitmap.c \
$(FREETYPE_FOLDER)/src/cache/ftcache.c \
$(FREETYPE_FOLDER)/src/base/ftdebug.c \
$(FREETYPE_FOLDER)/src/base/ftgasp.c \
$(FREETYPE_FOLDER)/src/base/ftglyph.c \
$(FREETYPE_FOLDER)/src/gzip/ftgzip.c \
$(FREETYPE_FOLDER)/src/base/ftinit.c \
$(FREETYPE_FOLDER)/src/lzw/ftlzw.c \
$(FREETYPE_FOLDER)/src/base/ftstroke.c \
$(FREETYPE_FOLDER)/src/smooth/smooth.c \
$(FREETYPE_FOLDER)/src/base/ftbbox.c \
$(FREETYPE_FOLDER)/src/base/ftmm.c \
$(FREETYPE_FOLDER)/src/base/ftpfr.c \
$(FREETYPE_FOLDER)/src/base/ftsynth.c \
$(FREETYPE_FOLDER)/src/base/fttype1.c \
$(FREETYPE_FOLDER)/src/base/ftwinfnt.c \
$(FREETYPE_FOLDER)/src/pcf/pcf.c \
$(FREETYPE_FOLDER)/src/pfr/pfr.c \
$(FREETYPE_FOLDER)/src/psaux/psaux.c \
$(FREETYPE_FOLDER)/src/pshinter/pshinter.c \
$(FREETYPE_FOLDER)/src/psnames/psnames.c \
$(FREETYPE_FOLDER)/src/raster/raster.c \
$(FREETYPE_FOLDER)/src/sfnt/sfnt.c \
$(FREETYPE_FOLDER)/src/truetype/truetype.c \
$(FREETYPE_FOLDER)/src/type1/type1.c \
$(FREETYPE_FOLDER)/src/cid/type1cid.c \
$(FREETYPE_FOLDER)/src/type42/type42.c \
$(FREETYPE_FOLDER)/src/winfonts/winfnt.c

LOCAL_CFLAGS := -DFT2_BUILD_LIBRARY
LOCAL_CFLAGS 	+= -ffast-math -O3 -funroll-loops

LOCAL_C_INCLUDES := $(FREETYPE_INCLUDE)
LOCAL_SRC_FILES :=  $(FREETYPE_SRC)

include $(BUILD_STATIC_LIBRARY)

#################

OBJDIR := obj
ROOT_CODE_FOLDER := $(FREETYPE_FOLDER)
LOCAL_CFLAGS += -march=armv6 -mfpu=vfp -mfloat-abi=hard

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
	
