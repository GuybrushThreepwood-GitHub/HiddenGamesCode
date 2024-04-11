# Copyright (C) 2012 Hidden Games
#

LOCAL_PATH := $(call my-dir)

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PROJECT_PATH := ../..
SDK_ROOT := $(PROJECT_PATH)/../SDK

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

OPENAL_FOLDER := $(SDK_ROOT)/openal-soft-1.15.1
LUA_FOLDER := $(SDK_ROOT)/lua-5.2.3
ZLIB_FOLDER := $(SDK_ROOT)/zlib-1.2.8
PNG_FOLDER := $(SDK_ROOT)/libpng-1.6.8
FREETYPE_FOLDER := $(SDK_ROOT)/freetype-2.6
VORBIS_FOLDER := $(SDK_ROOT)/libvorbis-1.3.3
OGG_FOLDER := $(SDK_ROOT)/libogg-1.3.1
ZZIPLIB_FOLDER := $(SDK_ROOT)/zziplib-0.13.59

BOX2D_FOLDER := $(SDK_ROOT)/box2d-2.3.0

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

OPENAL_INCLUDE := $(OPENAL_FOLDER)/include
LUA_INCLUDE := $(LUA_FOLDER)/include
ZLIB_INCLUDE := $(ZLIB_FOLDER)/include
PNG_INCLUDE := $(PNG_FOLDER)/include
FREETYPE_INCLUDE := $(FREETYPE_FOLDER)/include
VORBIS_INCLUDE := $(VORBIS_FOLDER)/include
OGG_INCLUDE := $(OGG_FOLDER)/include
ZZIPLIB_INCLUDE := $(ZZIPLIB_FOLDER)/include

BOX2D_INCLUDE := $(BOX2D_FOLDER)/include

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# Firewall #
include $(CLEAR_VARS)

LOCAL_MODULE    := firewall

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#BASE_FOLDER := $(call host-path, $(PROJECT_PATH)/../Base)
#BASE_INCLUDE := $(PROJECT_PATH)/Base $(OPENAL_INCLUDE) $(BOX2D_INCLUDE) $(LUA_INCLUDE) $(ZLIB_INCLUDE) $(PNG_INCLUDE) $(FREETYPE_INCLUDE) $(VORBIS_INCLUDE) $(OGG_INCLUDE) $(ZZIPLIB_INCLUDE)

BASE_FOLDER := $(PROJECT_PATH)/../Base
BASE_INCLUDE := -I$(PROJECT_PATH)/../Base -I$(OPENAL_INCLUDE) -I$(LUA_INCLUDE) -I$(ZLIB_INCLUDE) -I$(PNG_INCLUDE) -I$(FREETYPE_INCLUDE) -I$(VORBIS_INCLUDE) -I$(OGG_INCLUDE) -I$(ZZIPLIB_INCLUDE)
BASE_INCLUDE += -I$(BOX2D_INCLUDE)

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BASE_SRC := $(BASE_FOLDER)/Collision/AABB.cpp \
$(BASE_FOLDER)/Collision/OBB.cpp \
$(BASE_FOLDER)/Collision/Sphere.cpp \
$(BASE_FOLDER)/Core/rpi/RaspberryPiApp.cpp \
$(BASE_FOLDER)/Core/rpi/RaspberryPiMutex.cpp \
$(BASE_FOLDER)/Core/rpi/RaspberryPiThread.cpp \
$(BASE_FOLDER)/Core/App.cpp \
$(BASE_FOLDER)/Core/CoreFunctions.cpp \
$(BASE_FOLDER)/Core/Endian.cpp \
$(BASE_FOLDER)/Core/FileIO.cpp \
$(BASE_FOLDER)/Debug/FF/DebugPrint.cpp \
$(BASE_FOLDER)/Debug/GLSL/DebugPrintGLSL.cpp \
$(BASE_FOLDER)/Debug/Assertion.cpp \
$(BASE_FOLDER)/Debug/DebugLogging.cpp \
$(BASE_FOLDER)/Input/Input.cpp \
$(BASE_FOLDER)/Math/MathFunctions.cpp \
$(BASE_FOLDER)/Math/Matrix.cpp \
$(BASE_FOLDER)/Math/Quaternion.cpp \
$(BASE_FOLDER)/Math/RandomTables.cpp \
$(BASE_FOLDER)/Math/Vectors.cpp \
$(BASE_FOLDER)/Model/FF/Mesh.cpp \
$(BASE_FOLDER)/Model/FF/ModelHGA.cpp \
$(BASE_FOLDER)/Model/FF/ModelHGM.cpp \
$(BASE_FOLDER)/Model/FF/SkinMesh.cpp \
$(BASE_FOLDER)/Model/GLSL/MeshGLSL.cpp \
$(BASE_FOLDER)/Model/GLSL/ModelHGAGLSL.cpp \
$(BASE_FOLDER)/Model/GLSL/ModelHGMGLSL.cpp \
$(BASE_FOLDER)/Model/GLSL/SkinMeshGLSL.cpp \
$(BASE_FOLDER)/Model/Model.cpp \
$(BASE_FOLDER)/Physics/box2d/FF/Box2DDebugDraw.cpp \
$(BASE_FOLDER)/Physics/box2d/GLSL/Box2DDebugDrawGLSL.cpp \
$(BASE_FOLDER)/Physics/box2d/PhysicsWorldB2D.cpp \
$(BASE_FOLDER)/Physics/ode/FF/ODEPhysicsPrimitives.cpp \
$(BASE_FOLDER)/Physics/ode/GLSL/ODEPhysicsPrimitivesGLSL.cpp \
$(BASE_FOLDER)/Physics/ode/PhysicsWorldODE.cpp \
$(BASE_FOLDER)/Render/FF/OpenGLES/ExtensionsOES.cpp \
$(BASE_FOLDER)/Render/FF/OpenGLES/gluSupport.cpp \
$(BASE_FOLDER)/Render/FF/OpenGLES/OpenGLES.cpp \
$(BASE_FOLDER)/Render/FF/OpenGLES/RenderToTextureOES.cpp \
$(BASE_FOLDER)/Render/FF/OpenGLES/SpriteOES.cpp \
$(BASE_FOLDER)/Render/FF/OpenGLES/TextureLoadAndUploadOES.cpp \
$(BASE_FOLDER)/Render/FF/FreetypeFont.cpp \
$(BASE_FOLDER)/Render/FF/Primitives.cpp \
$(BASE_FOLDER)/Render/GLSL/FreetypeFontGLSL.cpp \
$(BASE_FOLDER)/Render/GLSL/glewES.cpp \
$(BASE_FOLDER)/Render/GLSL/gluSupportGLSL.cpp \
$(BASE_FOLDER)/Render/GLSL/OpenGLSL.cpp \
$(BASE_FOLDER)/Render/GLSL/PrimitivesGLSL.cpp \
$(BASE_FOLDER)/Render/GLSL/RenderToTextureGLSL.cpp \
$(BASE_FOLDER)/Render/GLSL/ShaderShared.cpp \
$(BASE_FOLDER)/Render/GLSL/TextureLoadAndUploadGLSL.cpp \
$(BASE_FOLDER)/Render/FreetypeFontZZip.c \
$(BASE_FOLDER)/Render/FreetypeZZipAccess.c \
$(BASE_FOLDER)/Render/OpenGLCommon.cpp \
$(BASE_FOLDER)/Render/Texture.cpp \
$(BASE_FOLDER)/Render/TextureAtlas.cpp \
$(BASE_FOLDER)/Render/TextureShared.cpp \
$(BASE_FOLDER)/Script/box2d/PhysicsAccessB2D.cpp \
$(BASE_FOLDER)/Script/BaseScriptSupport.cpp \
$(BASE_FOLDER)/Script/LuaScripting.cpp \
$(BASE_FOLDER)/Sound/rpi/RaspberryPiSound.cpp \
$(BASE_FOLDER)/Sound/rpi/RaspberryPiSoundStream.cpp \
$(BASE_FOLDER)/Sound/OpenAL.cpp \
$(BASE_FOLDER)/Sound/Sound.cpp \
$(BASE_FOLDER)/Sound/SoundCodecs.cpp \
$(BASE_FOLDER)/Sound/SoundLoadAndUpload.cpp \
$(BASE_FOLDER)/Sound/SoundManager.cpp \
$(BASE_FOLDER)/Sound/SoundStream.cpp \
$(BASE_FOLDER)/Support/rpi/RaspberryPiPurchase.cpp \
$(BASE_FOLDER)/Support/rpi/RaspberryPiScores.cpp \
$(BASE_FOLDER)/Support/Leaderboards.cpp \
$(BASE_FOLDER)/Support/Purchase.cpp \
$(BASE_FOLDER)/Support/Scores.cpp

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#GAME_FOLDER := $(call host-path, $(PROJECT_PATH)/../H8)
#GAME_INCLUDE := $(PROJECT_PATH)/H8 $(PROJECT_PATH)/H8/Source $(PROJECT_PATH)/H8/Source/xx_H8_xx

GAME_FOLDER := $(PROJECT_PATH)/../H8
GAME_INCLUDE := -I$(PROJECT_PATH)/../H8 -I$(PROJECT_PATH)/../H8/Source -I$(PROJECT_PATH)/../H8/Source/xx_H8_xx

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GAME_SRC := $(GAME_FOLDER)/Source/Audio/rpi/RaspberryPiAudioSystemBase.cpp \
$(GAME_FOLDER)/Source/Audio/AudioSystem.cpp \
$(GAME_FOLDER)/Source/Camera/Camera.cpp \
$(GAME_FOLDER)/Source/Camera/DebugCamera.cpp \
$(GAME_FOLDER)/Source/Camera/GameCamera.cpp \
$(GAME_FOLDER)/Source/Effects/ConsolePrint.cpp \
$(GAME_FOLDER)/Source/Effects/Emitter.cpp \
$(GAME_FOLDER)/Source/Effects/WaveGrid.cpp \
$(GAME_FOLDER)/Source/GameEffects/FullscreenEffects.cpp \
$(GAME_FOLDER)/Source/Profiles/ProfileManager.cpp \
$(GAME_FOLDER)/Source/Resources/AchievementList.cpp \
$(GAME_FOLDER)/Source/Resources/AnimatedResources.cpp \
$(GAME_FOLDER)/Source/Resources/EmitterResources.cpp \
$(GAME_FOLDER)/Source/Resources/FontResources.cpp \
$(GAME_FOLDER)/Source/Resources/IAPList.cpp \
$(GAME_FOLDER)/Source/Resources/ModelResources.cpp \
$(GAME_FOLDER)/Source/Resources/ResourceHelper.cpp \
$(GAME_FOLDER)/Source/Resources/SoundResources.cpp \
$(GAME_FOLDER)/Source/Resources/SpriteResources.cpp \
$(GAME_FOLDER)/Source/Resources/StringResources.cpp \
$(GAME_FOLDER)/Source/Resources/TextureResources.cpp \
$(GAME_FOLDER)/Source/StateManage/StateManager.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/Core/Core.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/EffectPool/EffectPool.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/Enemy/Enemy.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/Enemy/EnemyManager.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/GameStates/UI/AchievementUI.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/GameStates/UI/BackgroundUI.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/GameStates/UI/FrontendTitleScreenUI.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/GameStates/UI/GameOverUI.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/GameStates/UI/GameUI.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/GameStates/UI/MainMenuUI.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/GameStates/UI/PauseMenuUI.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/GameStates/UI/TextFormattingFuncs.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/GameStates/UI/UIBaseState.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/GameStates/UI/UIFileLoader.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/GameStates/MainGameState.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/GameStates/TitleScreenState.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/InputSystem/InputSystem.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/Physics/PhysicsContact.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/Player/Player.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/ScriptAccess/H8/H8Access.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/ScriptAccess/ScriptAccess.cpp \
$(GAME_FOLDER)/Source/xx_H8_xx/ScriptAccess/ScriptDataHolder.cpp \
$(GAME_FOLDER)/Source/H8.cpp \
$(GAME_FOLDER)/Source/GameSystems.cpp \
$(GAME_FOLDER)/Source/mainRaspberryPi.cpp

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LOCAL_CFLAGS := -Wno-psabi -DGL_GLEXT_PROTOTYPES=1 -DBASE_PLATFORM_RASPBERRYPI -DBASE_INPUT_NO_TOUCH_SUPPORT -DBASE_SUPPORT_OPENGLES -DBASE_SUPPORT_OGG -DBASE_SUPPORT_FREETYPE -DBASE_SUPPORT_SCRIPTING -DBASE_SUPPORT_BOX2D -DBASE_SUPPORT_FLURRY
#LOCAL_CFLAGS += -D_DEBUG 
LOCAL_CFLAGS += -ffast-math -O3 -funroll-loops -Wmultichar

LOCAL_C_INCLUDES := $(GAME_INCLUDE) $(BASE_INCLUDE)
LOCAL_SRC_FILES :=  $(GAME_SRC) $(BASE_SRC)

LOCAL_LDLIBS := -L/opt/vc/lib -lGLESv1_CM

LOCAL_STATIC_LIBRARIES := $(LUA_FOLDER)/libs/liblua.a $(PNG_FOLDER)/libs/libpng.a $(ZLIB_FOLDER)/libs/libzlib.a $(FREETYPE_FOLDER)/libs/libfreetype.a $(VORBIS_FOLDER)/libs/libvorbis.a $(OGG_FOLDER)/libs/libogg.a $(ZZIPLIB_FOLDER)/libs/libzziplib.a
LOCAL_STATIC_LIBRARIES += $(BOX2D_FOLDER)/libs/libbox2d.a

LOCAL_SHARED_LIBRARIES := -L$(OPENAL_FOLDER)/libs -lopenal

include $(BUILD_SHARED_LIBRARY)

#################

OBJDIR := obj

LOCAL_CFLAGS += -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM
LOCAL_CFLAGS += -march=armv6 -mfpu=vfp -mfloat-abi=hard
LOCAL_C_INCLUDES += -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/interface/vmcs_host/linux
LOCAL_LDLIBS += -Wl -lEGL -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt

FILTERED_OBJECTS := $(patsubst $(GAME_FOLDER)/%, $(OBJDIR)/%, $(LOCAL_SRC_FILES) ) 
TEMP_OBJECTS := $(FILTERED_OBJECTS)
FILTERED_OBJECTS = $(patsubst $(BASE_FOLDER)/%, $(OBJDIR)/%, $(TEMP_OBJECTS) ) 

C_OBJECTS := $(FILTERED_OBJECTS:.c=.o)
OBJECTS := $(C_OBJECTS:.cpp=.o)
OUTFOLDERS := $(dir $(OBJECTS) ) 

APPLICATION := ../../bin/$(LOCAL_MODULE)
MKDIR := mkdir

all: $(OBJDIR) $(APPLICATION)
		
$(APPLICATION): $(OBJECTS) 
	g++ -o $@ $(OBJECTS) $(LOCAL_LDLIBS) $(LOCAL_STATIC_LIBRARIES) $(LOCAL_SHARED_LIBRARIES)
	
$(OBJDIR): 
	$(MKDIR) -p $@
	$(MKDIR) -p $(OUTFOLDERS)
	
$(OBJDIR)/%.o : $(GAME_FOLDER)/%.c
	gcc $(LOCAL_CFLAGS) $(LOCAL_C_INCLUDES) -g -c $< -o $@
	
$(OBJDIR)/%.o : $(GAME_FOLDER)/%.cpp
	gcc $(LOCAL_CFLAGS) $(LOCAL_C_INCLUDES) -g -c $< -o $@

$(OBJDIR)/%.o : $(BASE_FOLDER)/%.c
	gcc $(LOCAL_CFLAGS) $(LOCAL_C_INCLUDES) -g -c $< -o $@
	
$(OBJDIR)/%.o : $(BASE_FOLDER)/%.cpp
	gcc $(LOCAL_CFLAGS) $(LOCAL_C_INCLUDES) -g -c $< -o $@
	
clean:
	$(RM) $(APPLICATION)
	$(RM) -rf $(OBJDIR)
	
	
