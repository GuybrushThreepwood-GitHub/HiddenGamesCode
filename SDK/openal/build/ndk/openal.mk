
# openal NDK makefile
# lib built dynamically

include $(CLEAR_VARS)

LOCAL_MODULE    := openal

OPENAL_FOLDER := $(call host-path, $(PROJECT_PATH)/../SDK/openal)
OPENAL_INCLUDE := $(PROJECT_PATH)/SDK/openal/src/android/include $(PROJECT_PATH)/SDK/openal/src/android/OpenAL32/Include

OPENAL_SRC := $(OPENAL_FOLDER)/src/android/Alc/ALc.c \
$(OPENAL_FOLDER)/src/android/Alc/alcConfig.c \
$(OPENAL_FOLDER)/src/android/Alc/alcEcho.c \
$(OPENAL_FOLDER)/src/android/Alc/alcModulator.c \
$(OPENAL_FOLDER)/src/android/Alc/alcReverb.c \
$(OPENAL_FOLDER)/src/android/Alc/AlcRing.c \
$(OPENAL_FOLDER)/src/android/Alc/alcThread.c \
$(OPENAL_FOLDER)/src/android/Alc/ALu.c \
$(OPENAL_FOLDER)/src/android/Alc/android.c \
$(OPENAL_FOLDER)/src/android/Alc/bs2b.c \
$(OPENAL_FOLDER)/src/android/Alc/null.c \
$(OPENAL_FOLDER)/src/android/OpenAL32/alAuxEffectSlot.c \
$(OPENAL_FOLDER)/src/android/OpenAL32/alBuffer.c \
$(OPENAL_FOLDER)/src/android/OpenAL32/alDataBuffer.c \
$(OPENAL_FOLDER)/src/android/OpenAL32/alEffect.c \
$(OPENAL_FOLDER)/src/android/OpenAL32/alError.c \
$(OPENAL_FOLDER)/src/android/OpenAL32/alExtension.c \
$(OPENAL_FOLDER)/src/android/OpenAL32/alFilter.c \
$(OPENAL_FOLDER)/src/android/OpenAL32/alListener.c \
$(OPENAL_FOLDER)/src/android/OpenAL32/alSource.c \
$(OPENAL_FOLDER)/src/android/OpenAL32/alState.c \
$(OPENAL_FOLDER)/src/android/OpenAL32/alThunk.c

#LOCAL_ARM_MODE   := arm
LOCAL_CFLAGS     := -DAL_BUILD_LIBRARY -DAL_ALEXT_PROTOTYPES
LOCAL_LDLIBS     := -llog -Wl,-s

LOCAL_C_INCLUDES := $(OPENAL_INCLUDE)
LOCAL_SRC_FILES :=  $(OPENAL_SRC)

include $(BUILD_SHARED_LIBRARY)
