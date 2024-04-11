
# ogg Raspberry Pi makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := ogg

OGG_FOLDER := ../..
OGG_INCLUDE := ../../include

OGG_SRC := $(OGG_FOLDER)/src/bitwise.c \
$(OGG_FOLDER)/src/framing.c

LOCAL_C_INCLUDES := $(OGG_INCLUDE)
LOCAL_SRC_FILES :=  $(OGG_SRC)
LOCAL_CFLAGS 	:= -ffast-math -O3 -funroll-loops
LOCAL_CFLAGS += -DLIBOGG_EXPORTS

include $(BUILD_STATIC_LIBRARY)

#################

OBJDIR := obj
ROOT_CODE_FOLDER := $(OGG_FOLDER)
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
	
