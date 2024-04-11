
# Box2D Raspberry Pi makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := box2d

BOX2D_FOLDER := ../..
BOX2D_INCLUDE := ../../include -I../../src

BOX2D_SRC := $(BOX2D_FOLDER)/src/Collision/Shapes/b2ChainShape.cpp \
$(BOX2D_FOLDER)/src/Collision/Shapes/b2CircleShape.cpp \
$(BOX2D_FOLDER)/src/Collision/Shapes/b2EdgeShape.cpp \
$(BOX2D_FOLDER)/src/Collision/Shapes/b2PolygonShape.cpp \
$(BOX2D_FOLDER)/src/Collision/b2BroadPhase.cpp \
$(BOX2D_FOLDER)/src/Collision/b2CollideCircle.cpp \
$(BOX2D_FOLDER)/src/Collision/b2CollideEdge.cpp \
$(BOX2D_FOLDER)/src/Collision/b2CollidePolygon.cpp \
$(BOX2D_FOLDER)/src/Collision/b2Collision.cpp \
$(BOX2D_FOLDER)/src/Collision/b2Distance.cpp \
$(BOX2D_FOLDER)/src/Collision/b2DynamicTree.cpp \
$(BOX2D_FOLDER)/src/Collision/b2TimeOfImpact.cpp \
$(BOX2D_FOLDER)/src/Common/b2BlockAllocator.cpp \
$(BOX2D_FOLDER)/src/Common/b2Draw.cpp \
$(BOX2D_FOLDER)/src/Common/b2Math.cpp \
$(BOX2D_FOLDER)/src/Common/b2Settings.cpp \
$(BOX2D_FOLDER)/src/Common/b2StackAllocator.cpp \
$(BOX2D_FOLDER)/src/Common/b2Timer.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Contacts/b2ChainAndCircleContact.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Contacts/b2ChainAndPolygonContact.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Contacts/b2CircleContact.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Contacts/b2Contact.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Contacts/b2ContactSolver.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Contacts/b2EdgeAndCircleContact.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Contacts/b2PolygonAndCircleContact.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Contacts/b2PolygonContact.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2DistanceJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2FrictionJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2GearJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2Joint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2MotorJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2MouseJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2PrismaticJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2PulleyJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2RevoluteJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2RopeJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2WeldJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2WheelJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/b2Body.cpp \
$(BOX2D_FOLDER)/src/Dynamics/b2ContactManager.cpp \
$(BOX2D_FOLDER)/src/Dynamics/b2Fixture.cpp \
$(BOX2D_FOLDER)/src/Dynamics/b2Island.cpp \
$(BOX2D_FOLDER)/src/Dynamics/b2World.cpp \
$(BOX2D_FOLDER)/src/Dynamics/b2WorldCallbacks.cpp \
$(BOX2D_FOLDER)/src/Rope/b2Rope.cpp

LOCAL_C_INCLUDES := $(BOX2D_INCLUDE)
LOCAL_SRC_FILES :=  $(BOX2D_SRC)

LOCAL_CFLAGS 	:= -ffast-math -O3 -funroll-loops

include $(BUILD_STATIC_LIBRARY)

#################

OBJDIR := obj
ROOT_CODE_FOLDER := $(BOX2D_FOLDER)
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
	
