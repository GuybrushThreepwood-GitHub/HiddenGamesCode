
# Lua NDK makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := lua

LUA_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/lua-5.2.3)
LUA_INCLUDE := $(SDK_INC_ROOT)/lua-5.2.3/include

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
