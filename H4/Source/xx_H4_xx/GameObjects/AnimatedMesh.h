
/*===================================================================
	File: AnimatedMesh.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __ANIMATEDMESH_H__
#define __ANIMATEDMESH_H__

#include "AppConsts.h"

#include "BaseObject.h"

class AnimatedMesh : public BaseObject
{
	public:
		AnimatedMesh( int objectId, math::Vec3& rot, int meshResourceIndex, lua_State* state );
		virtual ~AnimatedMesh();

		virtual int PostConstruction();

		void Update( float deltaTime );
		void Draw();

		void UpdateJoints( float deltaTime );
		void UpdateSkins();
		void SetAnimation( unsigned int nAnimId, int nFrameOffset = -1 );

	private:
		struct LocalAnim
		{
			/// time
			float animTimer;
			// animation identifier
			unsigned int nAnimId;
			/// framerate of the animation
			float fAnimationFramerate;
			/// total amount of time this animation plays for
			double dTotalAnimationTime;
			/// total number of joints exported into this file
			unsigned int nTotalJoints;
			/// total number of frames in this animation
			int nTotalFrames;
			/// timer for tracking this animation
			//core::app::CTimer animTimer;
			/// looping flag
			bool bLooping;
			/// current active frame
			int nCurrentFrame;
			/// current time
			double dCurrentTime;
			/// end of the animation time
			float fEndTime;
		};

		LocalAnim	m_LocalPrimaryAnim;
		mdl::BoneAnimation *m_pPrimaryAnim;

		unsigned int m_JointCount;
		mdl::TBoneJoint* m_pJoints;

		char m_FunctionBuffer[LUAFUNC_STRBUFFER];
		lua_State* m_LuaState;

		math::Vec3 m_Rot;
		mdl::ModelHGA* m_pModelData;
		collision::Sphere m_Sphere;

		int m_MeshResourceIndex;
		int m_SkinIndex;
		int m_RopeIndex;

		float m_CurrentTime;
		float m_Timer;
};

#endif // __ANIMATEDMESH_H__
