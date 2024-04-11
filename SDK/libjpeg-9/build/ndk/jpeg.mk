
# jpeg NDK makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := jpeg

JPEG_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/libjpeg-9)
JPEG_INCLUDE := $(SDK_INC_ROOT)/libjpeg-9/include

JPEG_SRC := $(JPEG_FOLDER)/src/jaricom.c \
$(JPEG_FOLDER)/src/jcapimin.c \
$(JPEG_FOLDER)/src/jcapistd.c \
$(JPEG_FOLDER)/src/jcarith.c \
$(JPEG_FOLDER)/src/jccoefct.c \
$(JPEG_FOLDER)/src/jccolor.c \
$(JPEG_FOLDER)/src/jcdctmgr.c \
$(JPEG_FOLDER)/src/jchuff.c \
$(JPEG_FOLDER)/src/jcinit.c \
$(JPEG_FOLDER)/src/jcmainct.c \
$(JPEG_FOLDER)/src/jcmarker.c \
$(JPEG_FOLDER)/src/jcmaster.c \
$(JPEG_FOLDER)/src/jcomapi.c \
$(JPEG_FOLDER)/src/jcparam.c \
$(JPEG_FOLDER)/src/jcprepct.c \
$(JPEG_FOLDER)/src/jcsample.c \
$(JPEG_FOLDER)/src/jctrans.c \
$(JPEG_FOLDER)/src/jdapimin.c \
$(JPEG_FOLDER)/src/jdapistd.c \
$(JPEG_FOLDER)/src/jdarith.c \
$(JPEG_FOLDER)/src/jdatadst.c \
$(JPEG_FOLDER)/src/jdatasrc.c \
$(JPEG_FOLDER)/src/jdcoefct.c \
$(JPEG_FOLDER)/src/jdcolor.c \
$(JPEG_FOLDER)/src/jddctmgr.c \
$(JPEG_FOLDER)/src/jdhuff.c \
$(JPEG_FOLDER)/src/jdinput.c \
$(JPEG_FOLDER)/src/jdmainct.c \
$(JPEG_FOLDER)/src/jdmarker.c \
$(JPEG_FOLDER)/src/jdmaster.c \
$(JPEG_FOLDER)/src/jdmerge.c \
$(JPEG_FOLDER)/src/jdpostct.c \
$(JPEG_FOLDER)/src/jdsample.c \
$(JPEG_FOLDER)/src/jdtrans.c \
$(JPEG_FOLDER)/src/jerror.c \
$(JPEG_FOLDER)/src/jfdctflt.c \
$(JPEG_FOLDER)/src/jfdctfst.c \
$(JPEG_FOLDER)/src/jfdctint.c \
$(JPEG_FOLDER)/src/jidctflt.c \
$(JPEG_FOLDER)/src/jidctfst.c \
$(JPEG_FOLDER)/src/jidctint.c \
$(JPEG_FOLDER)/src/jmemmgr.c \
$(JPEG_FOLDER)/src/jmemnobs.c \
$(JPEG_FOLDER)/src/jquant1.c \
$(JPEG_FOLDER)/src/jquant2.c \
$(JPEG_FOLDER)/src/jutils.c

LOCAL_C_INCLUDES := $(JPEG_INCLUDE)
LOCAL_SRC_FILES :=  $(JPEG_SRC)

LOCAL_CFLAGS 	:= -ffast-math -O3 -funroll-loops

include $(BUILD_STATIC_LIBRARY)
