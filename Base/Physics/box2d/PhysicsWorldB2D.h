
/*===================================================================
	File: PhysicsWorld.h
	Library: Physics (BOX2D)

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_BOX2D

#ifndef __PHYSICSWORLDB2D_H__
#define __PHYSICSWORLDB2D_H__

namespace physics
{
	class PhysicsWorldB2D
	{
		public:
			PhysicsWorldB2D();
			~PhysicsWorldB2D();
	 
			static void Create( b2Vec2& minBounds, b2Vec2& maxBounds, b2Vec2& gravity, bool enableDebugDraw = false );
			static void Destroy();

			static void DrawDebugData();

			static b2World* GetWorld();
			static Box2DDebugDraw* GetDrawer();

		private:
			static b2World* ms_World;
			static Box2DDebugDraw ms_DebugDraw;
	};
} // namespace physics

#endif // __PHYSICSWORLDB2D_H__

#endif // BASE_SUPPORT_BOX2D