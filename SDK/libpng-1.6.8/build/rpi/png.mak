
# png Raspberry Pi makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := png

PNG_FOLDER := ../..
PNG_INCLUDE := ../../include -I../../../zlib-1.2.8/include

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

#################

OBJDIR := obj
ROOT_CODE_FOLDER := $(PNG_FOLDER)
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
	
