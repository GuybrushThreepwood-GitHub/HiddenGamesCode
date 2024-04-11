
# zlib Raspberry Pi makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := zlib

#ZLIB_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/zlib-1.2.8)
#ZLIB_INCLUDE := $(SDK_INC_ROOT)/zlib-1.2.8/include

ZLIB_FOLDER := ../..
ZLIB_INCLUDE := ../../include

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

LOCAL_CFLAGS 	= -ffast-math -O3 -funroll-loops

include $(BUILD_STATIC_LIBRARY)

#################

OBJDIR := obj
ROOT_CODE_FOLDER := $(ZLIB_FOLDER)
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
	
