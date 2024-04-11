
# jsonbox Raspberry Pi makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := jsonbox

JSONBOX_FOLDER := ../..
JSONBOX_INCLUDE := ../../include

JSONBOX_SRC := $(JSONBOX_FOLDER)/src/Array.cpp \
$(JSONBOX_FOLDER)/src/Convert.cpp \
$(JSONBOX_FOLDER)/src/Escaper.cpp \
$(JSONBOX_FOLDER)/src/IndentCanceller.cpp \
$(JSONBOX_FOLDER)/src/Indenter.cpp \
$(JSONBOX_FOLDER)/src/Object.cpp \
$(JSONBOX_FOLDER)/src/SolidusEscaper.cpp \
$(JSONBOX_FOLDER)/src/Value.cpp

LOCAL_CFLAGS := -ffast-math -O3 -funroll-loops

LOCAL_C_INCLUDES := $(JSONBOX_INCLUDE)
LOCAL_SRC_FILES :=  $(JSONBOX_SRC)

include $(BUILD_STATIC_LIBRARY)

#################

OBJDIR := obj
ROOT_CODE_FOLDER := $(JSONBOX_FOLDER)
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
	
