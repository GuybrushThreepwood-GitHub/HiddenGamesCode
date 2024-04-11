
/*===================================================================
	File: Door.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __DOOR_H__
#define __DOOR_H__

#include "AppConsts.h"

#include "BaseObject.h"
#include "GameObjects/RoomInstance.h"

class Door : public BaseObject
{
	public:
		enum DoorProps
		{
			DOORTOGGLE = BaseObject::EXTRASTATE,
		};

		enum DoorState
		{
			DoorState_Closed=0,
			DoorState_Open,

			DoorState_Opening,
			DoorState_Closing,
		};

	public:
		Door( int objectId, int meshId, int type, bool hasReflection, RoomInstance* roomInstance, lua_State* state );
		virtual ~Door();

		virtual int PostConstruction();

		virtual void OnCreate();
		
		void Toggle();

		void Update( float deltaTime );
		void Draw();

	private:
		char m_FunctionBuffer[LUAFUNC_STRBUFFER];
		lua_State* m_LuaState;

		RoomInstance* m_RoomInstance;

		b2Body* m_pBody;
		b2PolygonShape m_PolyDef;
		b2FixtureDef m_FixDef;

		mdl::ModelHGM* m_pModel;
		collision::Sphere m_Sphere;
		collision::AABB m_AABB;

		math::Vec3 m_MinExtend;
		math::Vec3 m_MaxExtend;

		int m_Type;
		DoorState m_State;
		float m_CurrentHeight;
		float m_CurrentXExtend;
		bool m_Reflect;

		float m_ElevatorHalfWidth;

		ALuint m_DoorOpenAudio;
		ALuint m_DoorCloseAudio;
};

#endif // __DOOR_H__
