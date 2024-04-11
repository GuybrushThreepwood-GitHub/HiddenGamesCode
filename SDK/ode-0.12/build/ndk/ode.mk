
# ode NDK makefile
# lib built statically

include $(CLEAR_VARS)

LOCAL_MODULE    := ode

ODE_FOLDER := $(call host-path, $(SDK_SRC_ROOT)/ode-0.12)
ODE_INCLUDE := $(SDK_INC_ROOT)/ode-0.12/include $(SDK_INC_ROOT)/ode-0.12/src/ode $(SDK_INC_ROOT)/ode-0.12/src/ode/joints $(SDK_INC_ROOT)/ode-0.12/src/GIMPACT/include $(SDK_INC_ROOT)/ode-0.12/src/OPCODE $(SDK_INC_ROOT)/ode-0.12/src/OPCODE/Ice

ODE_SRC := $(ODE_FOLDER)/src/GIMPACT/src/gim_boxpruning.cpp \
$(ODE_FOLDER)/src/GIMPACT/src/gim_contact.cpp \
$(ODE_FOLDER)/src/GIMPACT/src/gim_math.cpp \
$(ODE_FOLDER)/src/GIMPACT/src/gim_memory.cpp \
$(ODE_FOLDER)/src/GIMPACT/src/gim_tri_tri_overlap.cpp \
$(ODE_FOLDER)/src/GIMPACT/src/gim_trimesh.cpp \
$(ODE_FOLDER)/src/GIMPACT/src/gim_trimesh_capsule_collision.cpp \
$(ODE_FOLDER)/src/GIMPACT/src/gim_trimesh_ray_collision.cpp \
$(ODE_FOLDER)/src/GIMPACT/src/gim_trimesh_sphere_collision.cpp \
$(ODE_FOLDER)/src/GIMPACT/src/gim_trimesh_trimesh_collision.cpp \
$(ODE_FOLDER)/src/GIMPACT/src/gimpact.cpp \
$(ODE_FOLDER)/src/ode/src/joints/amotor.cpp \
$(ODE_FOLDER)/src/ode/src/joints/ball.cpp \
$(ODE_FOLDER)/src/ode/src/joints/contact.cpp \
$(ODE_FOLDER)/src/ode/src/joints/fixed.cpp \
$(ODE_FOLDER)/src/ode/src/joints/hinge.cpp \
$(ODE_FOLDER)/src/ode/src/joints/hinge2.cpp \
$(ODE_FOLDER)/src/ode/src/joints/joint.cpp \
$(ODE_FOLDER)/src/ode/src/joints/lmotor.cpp \
$(ODE_FOLDER)/src/ode/src/joints/null.cpp \
$(ODE_FOLDER)/src/ode/src/joints/piston.cpp \
$(ODE_FOLDER)/src/ode/src/joints/plane2d.cpp \
$(ODE_FOLDER)/src/ode/src/joints/pr.cpp \
$(ODE_FOLDER)/src/ode/src/joints/pu.cpp \
$(ODE_FOLDER)/src/ode/src/joints/slider.cpp \
$(ODE_FOLDER)/src/ode/src/joints/universal.cpp \
$(ODE_FOLDER)/src/ode/src/array.cpp \
$(ODE_FOLDER)/src/ode/src/box.cpp \
$(ODE_FOLDER)/src/ode/src/capsule.cpp \
$(ODE_FOLDER)/src/ode/src/collision_cylinder_box.cpp \
$(ODE_FOLDER)/src/ode/src/collision_cylinder_plane.cpp \
$(ODE_FOLDER)/src/ode/src/collision_cylinder_sphere.cpp \
$(ODE_FOLDER)/src/ode/src/collision_cylinder_trimesh.cpp \
$(ODE_FOLDER)/src/ode/src/collision_kernel.cpp \
$(ODE_FOLDER)/src/ode/src/collision_quadtreespace.cpp \
$(ODE_FOLDER)/src/ode/src/collision_sapspace.cpp \
$(ODE_FOLDER)/src/ode/src/collision_space.cpp \
$(ODE_FOLDER)/src/ode/src/collision_transform.cpp \
$(ODE_FOLDER)/src/ode/src/collision_trimesh_box.cpp \
$(ODE_FOLDER)/src/ode/src/collision_trimesh_ccylinder.cpp \
$(ODE_FOLDER)/src/ode/src/collision_trimesh_disabled.cpp \
$(ODE_FOLDER)/src/ode/src/collision_trimesh_distance.cpp \
$(ODE_FOLDER)/src/ode/src/collision_trimesh_gimpact.cpp \
$(ODE_FOLDER)/src/ode/src/collision_trimesh_opcode.cpp \
$(ODE_FOLDER)/src/ode/src/collision_trimesh_plane.cpp \
$(ODE_FOLDER)/src/ode/src/collision_trimesh_ray.cpp \
$(ODE_FOLDER)/src/ode/src/collision_trimesh_sphere.cpp \
$(ODE_FOLDER)/src/ode/src/collision_trimesh_trimesh.cpp \
$(ODE_FOLDER)/src/ode/src/collision_trimesh_trimesh_new.cpp \
$(ODE_FOLDER)/src/ode/src/collision_util.cpp \
$(ODE_FOLDER)/src/ode/src/convex.cpp \
$(ODE_FOLDER)/src/ode/src/cylinder.cpp \
$(ODE_FOLDER)/src/ode/src/error.cpp \
$(ODE_FOLDER)/src/ode/src/export-dif.cpp \
$(ODE_FOLDER)/src/ode/src/fastdot.c \
$(ODE_FOLDER)/src/ode/src/fastldlt.c \
$(ODE_FOLDER)/src/ode/src/fastlsolve.c \
$(ODE_FOLDER)/src/ode/src/fastltsolve.c \
$(ODE_FOLDER)/src/ode/src/heightfield.cpp \
$(ODE_FOLDER)/src/ode/src/lcp.cpp \
$(ODE_FOLDER)/src/ode/src/mass.cpp \
$(ODE_FOLDER)/src/ode/src/mat.cpp \
$(ODE_FOLDER)/src/ode/src/matrix.cpp \
$(ODE_FOLDER)/src/ode/src/memory.cpp \
$(ODE_FOLDER)/src/ode/src/misc.cpp \
$(ODE_FOLDER)/src/ode/src/nextafterf.c \
$(ODE_FOLDER)/src/ode/src/obstack.cpp \
$(ODE_FOLDER)/src/ode/src/ode.cpp \
$(ODE_FOLDER)/src/ode/src/odeinit.cpp \
$(ODE_FOLDER)/src/ode/src/odemath.cpp \
$(ODE_FOLDER)/src/ode/src/odeou.cpp \
$(ODE_FOLDER)/src/ode/src/odetls.cpp \
$(ODE_FOLDER)/src/ode/src/plane.cpp \
$(ODE_FOLDER)/src/ode/src/quickstep.cpp \
$(ODE_FOLDER)/src/ode/src/ray.cpp \
$(ODE_FOLDER)/src/ode/src/rotation.cpp \
$(ODE_FOLDER)/src/ode/src/sphere.cpp \
$(ODE_FOLDER)/src/ode/src/step.cpp \
$(ODE_FOLDER)/src/ode/src/timer.cpp \
$(ODE_FOLDER)/src/ode/src/util.cpp \
$(ODE_FOLDER)/src/OPCODE/Ice/IceAABB.cpp \
$(ODE_FOLDER)/src/OPCODE/Ice/IceContainer.cpp \
$(ODE_FOLDER)/src/OPCODE/Ice/IceHPoint.cpp \
$(ODE_FOLDER)/src/OPCODE/Ice/IceIndexedTriangle.cpp \
$(ODE_FOLDER)/src/OPCODE/Ice/IceMatrix3x3.cpp \
$(ODE_FOLDER)/src/OPCODE/Ice/IceMatrix4x4.cpp \
$(ODE_FOLDER)/src/OPCODE/Ice/IceOBB.cpp \
$(ODE_FOLDER)/src/OPCODE/Ice/IcePlane.cpp \
$(ODE_FOLDER)/src/OPCODE/Ice/IcePoint.cpp \
$(ODE_FOLDER)/src/OPCODE/Ice/IceRandom.cpp \
$(ODE_FOLDER)/src/OPCODE/Ice/IceRay.cpp \
$(ODE_FOLDER)/src/OPCODE/Ice/IceRevisitedRadix.cpp \
$(ODE_FOLDER)/src/OPCODE/Ice/IceSegment.cpp \
$(ODE_FOLDER)/src/OPCODE/Ice/IceTriangle.cpp \
$(ODE_FOLDER)/src/OPCODE/Ice/IceUtils.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_AABBCollider.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_AABBTree.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_BaseModel.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_Collider.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_Common.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_HybridModel.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_LSSCollider.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_MeshInterface.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_Model.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_OBBCollider.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_OptimizedTree.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_Picking.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_PlanesCollider.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_RayCollider.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_SphereCollider.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_TreeBuilders.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_TreeCollider.cpp \
$(ODE_FOLDER)/src/OPCODE/OPC_VolumeCollider.cpp \
$(ODE_FOLDER)/src/OPCODE/Opcode.cpp \

LOCAL_CFLAGS 	:= -DODE_LIB
LOCAL_CFLAGS 	+= -ffast-math -O3 -funroll-loops

LOCAL_C_INCLUDES := $(ODE_INCLUDE)
LOCAL_SRC_FILES :=  $(ODE_SRC)

include $(BUILD_STATIC_LIBRARY)
