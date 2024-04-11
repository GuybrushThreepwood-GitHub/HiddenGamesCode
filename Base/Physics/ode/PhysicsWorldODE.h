
/*===================================================================
	File: PhysicsWorldODE.h
	Library: Physics (ODE)

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_ODE

#ifndef __PHYSICSWORLDODE_H__
#define __PHYSICSWORLDODE_H__

namespace physics
{
	class PhysicsWorldODE
	{
		public:
			PhysicsWorldODE();
			~PhysicsWorldODE();
	 
			static void Create();
			static void Destroy();

			static dWorldID GetWorld();
			static void SetSpace( dSpaceID spaceId );
			static void SetEditorSpace( dSpaceID spaceId );
			static void SetRayCastSpace( dSpaceID spaceId );
			static dSpaceID GetSpace();
			static dSpaceID GetEditorSpace();
			static dSpaceID GetRayCastSpace();
			static dJointGroupID GetJointGroup();

			static void SetPhysicsState( bool state );
			static bool GetPhysicsState();

		private:
			static dWorldID ms_WorldID;
			static dSpaceID ms_SpaceID;
			static dSpaceID ms_EditorSpaceID;
			static dSpaceID ms_RayCastSpaceID;
			static dJointGroupID ms_JointGroupID;

	};
} // namespace physics

#endif // __PHYSICSWORLDODE_H__

#endif // BASE_SUPPORT_ODE