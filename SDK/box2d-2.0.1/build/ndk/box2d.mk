
# Box2D NDK makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := box2d

BOX2D_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/box2d-2.0.1)
BOX2D_INCLUDE := $(SDK_INC_ROOT)/box2d-2.0.1/include $(SDK_INC_ROOT)/box2d-2.0.1/src

BOX2D_SRC := $(BOX2D_FOLDER)/src/Collision/Shapes/b2CircleShape.cpp \
$(BOX2D_FOLDER)/src/Collision/Shapes/b2PolygonShape.cpp \
$(BOX2D_FOLDER)/src/Collision/Shapes/b2Shape.cpp \
$(BOX2D_FOLDER)/src/Collision/b2BroadPhase.cpp \
$(BOX2D_FOLDER)/src/Collision/b2CollideCircle.cpp \
$(BOX2D_FOLDER)/src/Collision/b2CollidePoly.cpp \
$(BOX2D_FOLDER)/src/Collision/b2Collision.cpp \
$(BOX2D_FOLDER)/src/Collision/b2Distance.cpp \
$(BOX2D_FOLDER)/src/Collision/b2PairManager.cpp \
$(BOX2D_FOLDER)/src/Collision/b2TimeOfImpact.cpp \
$(BOX2D_FOLDER)/src/Common/b2BlockAllocator.cpp \
$(BOX2D_FOLDER)/src/Common/b2Math.cpp \
$(BOX2D_FOLDER)/src/Common/b2Settings.cpp \
$(BOX2D_FOLDER)/src/Common/b2StackAllocator.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Contacts/b2CircleContact.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Contacts/b2Contact.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Contacts/b2ContactSolver.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Contacts/b2PolyAndCircleContact.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Contacts/b2PolyContact.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2DistanceJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2GearJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2Joint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2MouseJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2PrismaticJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2PulleyJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/Joints/b2RevoluteJoint.cpp \
$(BOX2D_FOLDER)/src/Dynamics/b2Body.cpp \
$(BOX2D_FOLDER)/src/Dynamics/b2ContactManager.cpp \
$(BOX2D_FOLDER)/src/Dynamics/b2Island.cpp \
$(BOX2D_FOLDER)/src/Dynamics/b2World.cpp \
$(BOX2D_FOLDER)/src/Dynamics/b2WorldCallbacks.cpp

LOCAL_C_INCLUDES := $(BOX2D_INCLUDE)
LOCAL_SRC_FILES :=  $(BOX2D_SRC)

LOCAL_CFLAGS 	:= -ffast-math -O3 -funroll-loops

include $(BUILD_STATIC_LIBRARY)
