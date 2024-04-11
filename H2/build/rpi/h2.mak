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
ZZIPLIB_FOLDER := $(SDK_ROOT)/zziplib-0.13.59

BOX2D_FOLDER := $(SDK_ROOT)/box2d-2.0.1

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

OPENAL_INCLUDE := $(OPENAL_FOLDER)/include
LUA_INCLUDE := $(LUA_FOLDER)/include
ZLIB_INCLUDE := $(ZLIB_FOLDER)/include
PNG_INCLUDE := $(PNG_FOLDER)/include
FREETYPE_INCLUDE := $(FREETYPE_FOLDER)/include
ZZIPLIB_INCLUDE := $(ZZIPLIB_FOLDER)/include

BOX2D_INCLUDE := $(BOX2D_FOLDER)/include

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# Cabby (Full) #
include $(CLEAR_VARS)

LOCAL_MODULE    := cabby

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#BASE_FOLDER := $(call host-path, $(PROJECT_PATH)/../../Base)
#BASE_INCLUDE := $(PROJECT_PATH)/../Base $(OPENAL_INCLUDE) $(BOX2D_INCLUDE) $(LUA_INCLUDE) $(ZLIB_INCLUDE) $(PNG_INCLUDE) $(FREETYPE_INCLUDE) $(ZZIPLIB_INCLUDE)

BASE_FOLDER := $(PROJECT_PATH)/../Base
BASE_INCLUDE := -I$(PROJECT_PATH)/../Base -I$(OPENAL_INCLUDE) -I$(LUA_INCLUDE) -I$(ZLIB_INCLUDE) -I$(PNG_INCLUDE) -I$(FREETYPE_INCLUDE) -I$(ZZIPLIB_INCLUDE)
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

#GAME_FOLDER := $(call host-path, $(PROJECT_PATH)/../../H2)
#GAME_INCLUDE := $(PROJECT_PATH)/../H2 $(PROJECT_PATH)/../H2/Source $(PROJECT_PATH)/../H2/Source/xx_Cabby_xx

GAME_FOLDER := $(PROJECT_PATH)/../H2
GAME_INCLUDE := -I$(PROJECT_PATH)/../H2 -I$(PROJECT_PATH)/../H2/Source -I$(PROJECT_PATH)/../H2/Source/xx_Cabby_xx

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GAME_SRC := $(GAME_FOLDER)/Source/Audio/rpi/RaspberryPiAudioSystemBase.cpp \
$(GAME_FOLDER)/Source/Audio/AudioSystem.cpp \
$(GAME_FOLDER)/Source/Audio/CustomerAudio.cpp \
$(GAME_FOLDER)/Source/Audio/Sfx.cpp \
$(GAME_FOLDER)/Source/Camera/Camera.cpp \
$(GAME_FOLDER)/Source/Camera/DebugCamera.cpp \
$(GAME_FOLDER)/Source/Camera/GameCamera.cpp \
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
$(GAME_FOLDER)/Source/xx_Cabby_xx/Customers/Customer.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Customers/CustomerAnimationLookup.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Customers/CustomerManagement.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/AchievementUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/ControlsPhoneRetinaUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/ControlsPhoneSDUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/ControlsTabletRetinaUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/ControlsTabletSDUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/ControlsUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/FrontendInAppPurchaseUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/FrontendItemSelectUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/FrontendPromptsUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/FrontendUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/GameCompleteUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/GameOverArcadeUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/GameOverCareerContinueUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/GameOverCareerUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/HudUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/IconsPhoneRetinaUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/IconsPhoneSDUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/IconsTabletSDUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/IconsUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/LevelCompleteArcadeUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/LevelCompleteCareerUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/LevelStartUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/MainGameUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/PauseUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/TextFormattingFuncs.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/TimeUpUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/TitleScreenUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/UIBaseState.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/UIFileLoader.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UI/UnlockUI.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/MainGameState.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/SpecialEndingState.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/TitleScreenState.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/GameStates/UnlockState.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/InputSystem/InputSystem.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Level/Level.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Enemies/Bomb.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Enemies/BouncingBomb.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Enemies/DustDevil.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Enemies/Fish.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Enemies/Mine.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Enemies/Piranha.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Enemies/PufferFish.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Enemies/Rat.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Enemies/Sentry.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Objects/DoorSlideDown.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Objects/DoorSlideLeft.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Objects/DoorSlideRight.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Objects/DoorSlideUp.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Objects/HShape.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Objects/IShape.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Objects/Port.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Objects/UShape.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Objects/XShape.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Objects/YShape.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/Box2DDebugDraw.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Physics/PhysicsWorld.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/Player/Player.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/ScriptAccess/Cabby/CabbyAccess.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/ScriptAccess/Cabby/FogAccess.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/ScriptAccess/Cabby/LightAccess.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/ScriptAccess/Cabby/VehicleAccess.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/ScriptAccess/PhysicsAccess.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/ScriptAccess/ScriptAccess.cpp \
$(GAME_FOLDER)/Source/xx_Cabby_xx/ScriptAccess/ScriptDataHolder.cpp \
$(GAME_FOLDER)/Source/Cabby.cpp \
$(GAME_FOLDER)/Source/GameSystems.cpp \
$(GAME_FOLDER)/Source/mainRaspberryPi.cpp

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LOCAL_CFLAGS := -Wno-psabi -DGL_GLEXT_PROTOTYPES=1 -DBASE_PLATFORM_RASPBERRYPI -DBASE_INPUT_NO_TOUCH_SUPPORT -DBASE_SUPPORT_OPENGLES -DBASE_SUPPORT_FREETYPE -DBASE_SUPPORT_SCRIPTING -DBASE_SUPPORT_FLURRY -DBASE_SUPPORT_KIIP 
LOCAL_CFLAGS += -ffast-math -O3 -funroll-loops
#LOCAL_CFLAGS += -D_DEBUG

LOCAL_C_INCLUDES := $(GAME_INCLUDE) $(BASE_INCLUDE)
LOCAL_SRC_FILES :=  $(GAME_SRC) $(BASE_SRC)

LOCAL_LDLIBS := -L/opt/vc/lib -lGLESv1_CM

LOCAL_STATIC_LIBRARIES := $(LUA_FOLDER)/libs/liblua.a $(PNG_FOLDER)/libs/libpng.a $(ZLIB_FOLDER)/libs/libzlib.a $(FREETYPE_FOLDER)/libs/libfreetype.a $(ZZIPLIB_FOLDER)/libs/libzziplib.a
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
	
