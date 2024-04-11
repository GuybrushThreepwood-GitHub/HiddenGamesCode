
# Lua Raspberry Pi makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := lua

LUA_FOLDER := ../..
LUA_INCLUDE := ../../include

LUA_SRC := $(LUA_FOLDER)/src/lapi.c \
$(LUA_FOLDER)/src/lcode.c \
$(LUA_FOLDER)/src/lctype.c \
$(LUA_FOLDER)/src/ldebug.c \
$(LUA_FOLDER)/src/ldo.c \
$(LUA_FOLDER)/src/ldump.c \
$(LUA_FOLDER)/src/lfunc.c \
$(LUA_FOLDER)/src/lgc.c \
$(LUA_FOLDER)/src/llex-android.c \
$(LUA_FOLDER)/src/lmem.c \
$(LUA_FOLDER)/src/lobject.c \
$(LUA_FOLDER)/src/lopcodes.c \
$(LUA_FOLDER)/src/lparser.c \
$(LUA_FOLDER)/src/lstate.c \
$(LUA_FOLDER)/src/lstring.c \
$(LUA_FOLDER)/src/ltable.c \
$(LUA_FOLDER)/src/ltm.c \
$(LUA_FOLDER)/src/lundump.c \
$(LUA_FOLDER)/src/lvm.c \
$(LUA_FOLDER)/src/lzio.c \
$(LUA_FOLDER)/src/lauxlib.c \
$(LUA_FOLDER)/src/lbaselib.c \
$(LUA_FOLDER)/src/lbitlib.c \
$(LUA_FOLDER)/src/lcorolib.c \
$(LUA_FOLDER)/src/ldblib.c \
$(LUA_FOLDER)/src/linit.c \
$(LUA_FOLDER)/src/liolib.c \
$(LUA_FOLDER)/src/lmathlib.c \
$(LUA_FOLDER)/src/loadlib.c \
$(LUA_FOLDER)/src/loslib.c \
$(LUA_FOLDER)/src/lstrlib.c \
$(LUA_FOLDER)/src/ltablib.c

LOCAL_C_INCLUDES := $(LUA_INCLUDE)
LOCAL_SRC_FILES :=  $(LUA_SRC)

LOCAL_CFLAGS 	:= -ffast-math -O3 -funroll-loops -DLUA_ANSI

include $(BUILD_STATIC_LIBRARY)

#################

OBJDIR := obj
ROOT_CODE_FOLDER := $(LUA_FOLDER)
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
	
