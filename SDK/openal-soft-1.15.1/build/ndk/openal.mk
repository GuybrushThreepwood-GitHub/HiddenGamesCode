
# openal NDK makefile
# lib built dynamically

include $(CLEAR_VARS)

LOCAL_MODULE    := openal

OPENAL_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/openal-soft-1.15.1)
OPENAL_INCLUDE := $(SDK_INC_ROOT)/openal-soft-1.15.1/build/ndk $(SDK_INC_ROOT)/openal-soft-1.15.1/include $(SDK_INC_ROOT)/openal-soft-1.15.1/src/OpenAL32/Include

OPENAL_SRC := $(OPENAL_FOLDER)/src/Alc/ALc.c \
$(OPENAL_FOLDER)/src/Alc/alcConfig.c \
$(OPENAL_FOLDER)/src/Alc/alcDedicated.c \
$(OPENAL_FOLDER)/src/Alc/alcEcho.c \
$(OPENAL_FOLDER)/src/Alc/alcModulator.c \
$(OPENAL_FOLDER)/src/Alc/alcReverb.c \
$(OPENAL_FOLDER)/src/Alc/alcRing.c \
$(OPENAL_FOLDER)/src/Alc/alcThread.c \
$(OPENAL_FOLDER)/src/Alc/ALu.c \
$(OPENAL_FOLDER)/src/Alc/bs2b.c \
$(OPENAL_FOLDER)/src/Alc/helpers.c \
$(OPENAL_FOLDER)/src/Alc/hrtf.c \
$(OPENAL_FOLDER)/src/Alc/mixer.c \
$(OPENAL_FOLDER)/src/Alc/mixer_c.c \
$(OPENAL_FOLDER)/src/Alc/panning.c \
$(OPENAL_FOLDER)/src/Alc/backends/loopback.c \
$(OPENAL_FOLDER)/src/Alc/backends/null.c \
$(OPENAL_FOLDER)/src/Alc/backends/opensl.c \
$(OPENAL_FOLDER)/src/OpenAL32/alAuxEffectSlot.c \
$(OPENAL_FOLDER)/src/OpenAL32/alBuffer.c \
$(OPENAL_FOLDER)/src/OpenAL32/alEffect.c \
$(OPENAL_FOLDER)/src/OpenAL32/alError.c \
$(OPENAL_FOLDER)/src/OpenAL32/alExtension.c \
$(OPENAL_FOLDER)/src/OpenAL32/alFilter.c \
$(OPENAL_FOLDER)/src/OpenAL32/alListener.c \
$(OPENAL_FOLDER)/src/OpenAL32/alSource.c \
$(OPENAL_FOLDER)/src/OpenAL32/alState.c \
$(OPENAL_FOLDER)/src/OpenAL32/alThunk.c

LOCAL_CFLAGS     := -DAL_BUILD_LIBRARY -DAL_ALEXT_PROTOTYPES
LOCAL_CFLAGS 	+= -ffast-math -O3 -funroll-loops

LOCAL_LDLIBS     := -lOpenSLES -llog -Wl,-s

LOCAL_C_INCLUDES := $(OPENAL_INCLUDE)
LOCAL_SRC_FILES :=  $(OPENAL_SRC)

include $(BUILD_SHARED_LIBRARY)
