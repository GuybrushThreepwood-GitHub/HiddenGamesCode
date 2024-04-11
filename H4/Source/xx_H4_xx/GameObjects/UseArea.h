
/*===================================================================
	File: UseArea.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __USEAREA_H__
#define __USEAREA_H__

#include "AppConsts.h"

#include "BaseObject.h"

class UseArea : public BaseObject
{
	public:
		UseArea( int objectId, lua_State* state, const collision::Sphere& s, float angle );
		virtual ~UseArea();

		virtual void OnCreate();
		virtual void OnActivate();
		virtual void OnUse();

		const collision::Sphere& GetSphere()			{ return m_Sphere; }
		float GetAngle()								{ return m_Angle; }

	private:
		char m_FunctionBuffer[LUAFUNC_STRBUFFER];

		collision::Sphere m_Sphere;
		float m_Angle;
};

#endif // __USEAREA_H__
