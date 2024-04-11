# Copyright (C) 2011 Hidden Games
#

LOCAL_PATH := $(call my-dir)

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PROJECT_PATH := ../../..
SDK_MK_ROOT := $(PROJECT_PATH)/SDK

SDK_SRC_ROOT := $(PROJECT_PATH)/../SDK/
SDK_INC_ROOT := $(PROJECT_PATH)/SDK

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

include $(SDK_MK_ROOT)/box2d-2.3.0/build/ndk/box2d.mk
include $(SDK_MK_ROOT)/lua-5.2.3/build/ndk/lua.mk
include $(SDK_MK_ROOT)/zlib-1.2.8/build/ndk/zlib.mk
include $(SDK_MK_ROOT)/libpng-1.6.8/build/ndk/png.mk
include $(SDK_MK_ROOT)/freetype-2.6/build/ndk/freetype.mk
include $(SDK_MK_ROOT)/libvorbis-1.3.3/build/ndk/vorbis.mk
include $(SDK_MK_ROOT)/libogg-1.3.1/build/ndk/ogg.mk
include $(SDK_MK_ROOT)/openal-soft-1.15.1/build/ndk/openal.mk
include $(SDK_MK_ROOT)/zziplib-0.13.59/build/ndk/zziplib.mk

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# Prisoner 84 #
include $(CLEAR_VARS)

LOCAL_MODULE    := prisoner84

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BASE_FOLDER := $(call host-path, $(PROJECT_PATH)/../Base)
BASE_INCLUDE := $(PROJECT_PATH)/Base $(OPENAL_INCLUDE) $(BOX2D_INCLUDE) $(LUA_INCLUDE) $(ZLIB_INCLUDE) $(PNG_INCLUDE) $(FREETYPE_INCLUDE) $(VORBIS_INCLUDE) $(OGG_INCLUDE) $(ZZIPLIB_INCLUDE)

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

BASE_SRC := $(BASE_FOLDER)/Collision/AABB.cpp \
$(BASE_FOLDER)/Collision/OBB.cpp \
$(BASE_FOLDER)/Collision/Sphere.cpp \
$(BASE_FOLDER)/Core/aos/AndroidApp.cpp \
$(BASE_FOLDER)/Core/aos/AndroidMutex.cpp \
$(BASE_FOLDER)/Core/aos/AndroidThread.cpp \
$(BASE_FOLDER)/Core/aos/jniCore.cpp \
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
$(BASE_FOLDER)/Render/FF/AdvertBar.cpp \
$(BASE_FOLDER)/Render/FF/FreetypeFont.cpp \
$(BASE_FOLDER)/Render/FF/Primitives.cpp \
$(BASE_FOLDER)/Render/GLSL/AdvertBarGLSL.cpp \
$(BASE_FOLDER)/Render/GLSL/FreetypeFontGLSL.cpp \
$(BASE_FOLDER)/Render/GLSL/glewES.cpp \
$(BASE_FOLDER)/Render/GLSL/gluSupportGLSL.cpp \
$(BASE_FOLDER)/Render/GLSL/OpenGLSL.cpp \
$(BASE_FOLDER)/Render/GLSL/PrimitivesGLSL.cpp \
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
$(BASE_FOLDER)/Sound/aos/AndroidSound.cpp \
$(BASE_FOLDER)/Sound/aos/AndroidSoundStream.cpp \
$(BASE_FOLDER)/Sound/OpenAL.cpp \
$(BASE_FOLDER)/Sound/Sound.cpp \
$(BASE_FOLDER)/Sound/SoundCodecs.cpp \
$(BASE_FOLDER)/Sound/SoundLoadAndUpload.cpp \
$(BASE_FOLDER)/Sound/SoundManager.cpp \
$(BASE_FOLDER)/Sound/SoundStream.cpp \
$(BASE_FOLDER)/Support/aos/AndroidPurchase.cpp \
$(BASE_FOLDER)/Support/aos/AndroidScores.cpp \
$(BASE_FOLDER)/Support/Leaderboards.cpp \
$(BASE_FOLDER)/Support/Purchase.cpp \
$(BASE_FOLDER)/Support/Scores.cpp

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GAME_FOLDER := $(call host-path, $(PROJECT_PATH)/../H4)
GAME_INCLUDE := $(PROJECT_PATH)/H4 $(PROJECT_PATH)/H4/Source $(PROJECT_PATH)/H4/Source/xx_H4_xx

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GAME_SRC := $(GAME_FOLDER)/Source/Audio/aos/AndroidAudioSystemBase.cpp \
$(GAME_FOLDER)/Source/Audio/AudioSystem.cpp \
$(GAME_FOLDER)/Source/Camera/Camera.cpp \
$(GAME_FOLDER)/Source/Camera/DebugCamera.cpp \
$(GAME_FOLDER)/Source/Camera/GameCamera.cpp \
$(GAME_FOLDER)/Source/Effects/Emitter.cpp \
$(GAME_FOLDER)/Source/InputSystem/InputSystem.cpp \
$(GAME_FOLDER)/Source/NavigationMesh/NavigationCell.cpp \
$(GAME_FOLDER)/Source/NavigationMesh/NavigationMesh.cpp \
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
$(GAME_FOLDER)/Source/ScriptedSequence/ScriptedSequencePlayer.cpp \
$(GAME_FOLDER)/Source/StateManage/StateManager.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Decals/DecalSystem.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Enemy/CrawlingSpider.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Enemy/CrawlingSpiderAnimationLookup.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Enemy/Enemy.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Enemy/EnemyAnimationLookup.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Enemy/EnemyManagement.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Enemy/FloatingHead.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Enemy/Gib.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Enemy/HangingSpider.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Enemy/HangingSpiderAnimationlookup.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Enemy/Insects.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/EventManage/Event.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameEffects/FullscreenEffects.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameEffects/InsectAttack.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameObjects/Script/ScriptBaseObject.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameObjects/AnimatedMesh.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameObjects/BaseObject.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameObjects/Door.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameObjects/RoomInstance.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameObjects/UseArea.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameStates/UI/AchievementUI.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameStates/UI/MainStateUI.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameStates/UI/MainStateUINormal.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameStates/UI/MainStateUITablet.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameStates/UI/UIBaseState.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameStates/UI/UIFileLoader.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameStates/CreditRollState.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameStates/EndingState.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameStates/FrontendState.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameStates/IntroOutroState.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameStates/LoadingState.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/GameStates/MainState.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Level/Level.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/MicroGame/MicroGameCardSwipe.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/MicroGame/MicroGameCriminalDocs.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/MicroGame/MicroGameDials.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/MicroGame/MicroGameInventory.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/MicroGame/MicroGameKeylock.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/MicroGame/MicroGameKeypad.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/MicroGame/MicroGameLiftButtons.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/MicroGame/MicroGameMap.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/MicroGame/MicroGamePickupItem.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/MicroGame/MicroGameSliderHold.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/MicroGame/MicroGameThumbScan.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/MicroGame/MicroGameUseItem.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/MicroGame/MicroGameWaterTightDoor.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/MicroGame/MicroGameWiring.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Physics/PhysicsContact.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Player/Scripted/ScriptedPlayer.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Player/Scripted/ScriptedPlayerAnimationLookup.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Player/Bullet.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Player/InventoryManagement.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Player/Player.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/Player/PlayerAnimationLookup.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/ScriptAccess/Emitter/EmitterAccess.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/ScriptAccess/H4/Inventory/InventoryAccess.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/ScriptAccess/H4/Room/RoomAccess.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/ScriptAccess/H4/H4Access.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/ScriptAccess/Sound/SoundAccess.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/ScriptAccess/ScriptAccess.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/ScriptAccess/ScriptDataHolder.cpp \
$(GAME_FOLDER)/Source/xx_H4_xx/ScriptAccess/WorldAccess.cpp \
$(GAME_FOLDER)/Source/GameSystems.cpp \
$(GAME_FOLDER)/Source/H4.cpp \
$(GAME_FOLDER)/Source/mainAndroid.cpp

## ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LOCAL_CFLAGS := -Wno-psabi -DGL_GLEXT_PROTOTYPES=1 -DBASE_PLATFORM_ANDROID -DBASE_INPUT_NO_TOUCH_SUPPORT -DBASE_SUPPORT_OPENGLES -DBASE_SUPPORT_OGG -DBASE_SUPPORT_FREETYPE -DBASE_SUPPORT_SCRIPTING -DBASE_SUPPORT_BOX2D -DBASE_SUPPORT_FLURRY
LOCAL_CFLAGS += -ffast-math -O3 -funroll-loops
#LOCAL_CFLAGS += -D_DEBUG

LOCAL_C_INCLUDES := $(GAME_INCLUDE) $(BASE_INCLUDE)
LOCAL_SRC_FILES :=  $(GAME_SRC) $(BASE_SRC)

LOCAL_LDLIBS := -lGLESv1_CM -lOpenSLES -ldl -llog

LOCAL_STATIC_LIBRARIES := box2d lua png zlib freetype vorbis ogg zziplib
LOCAL_SHARED_LIBRARIES := openal

include $(BUILD_SHARED_LIBRARY)
