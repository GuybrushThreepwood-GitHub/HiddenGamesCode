
/*===================================================================
	File: HangingSpider.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"
#include "PhysicsBase.h"

#include "AppConsts.h"

#include "Physics/PhysicsContact.h"
#include "Player/Player.h"

#include "Resources/SoundResources.h"
#include "Resources/SpriteResources.h"
#include "Resources/EmitterResources.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"

#include "Enemy/HangingSpiderAnimationLookup.h"
#include "Enemy/HangingSpider.h"

namespace
{
	const float CULLING_RADIUS = 0.5f;
	const int BOUNDING_JOINT_INDEX = 1;

	const float DROP_TIME_MIN = 3.0f;
	const float DROP_TIME_MAX = 10.0f;

	const float RESET_TIME_MIN = 1.0f;
	const float RESET_TIME_MAX = 3.0f;

	const int SWING_MIN = 1;
	const int SWING_MAX = 3;

	const int EMITTER_DEATH = 0;

	const float DEAD_TIME = 3.0f;

	b2Vec2 zeroVec2(0.0f,0.0f);
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

#ifdef _DEBUG
	math::Vec3 vaPoints[256];
	//int pointIndex = 0;
#endif // _DEBUG
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
HangingSpider::HangingSpider(  )
{
	int i=0;

	m_pModelData = 0;
	m_pPrimaryAnim = 0;
	m_pPolledAnim = 0;
	m_JointCount = 0;
	m_pJoints = 0;

	m_State = HangingSpiderState_InActive;
	m_DrawState = true;
	
	m_Pos = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_PrevPos = m_Pos;
	m_RotationAngle = 0.0f;

	m_BoundSphere.vCenterPoint = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_BoundSphere.fRadius = CULLING_RADIUS;

	m_UnProjectedPoint = math::Vec2( 0.0f, 0.0f );

	m_LastDeltaTime = 0.0f;

	for( i=0; i < MAX_HANGINGSPIDER_EMITTERS; ++i )
		m_pEmitters[i] = 0;

	m_DropTimer = math::RandomNumber( DROP_TIME_MIN, DROP_TIME_MAX );
	m_ResetTimer = math::RandomNumber( RESET_TIME_MIN, RESET_TIME_MAX );

	m_NumSwings = math::RandomNumber( SWING_MIN, SWING_MAX );
	m_CompleteSwings = 0;

	m_DeadTimer = DEAD_TIME;

	m_HurtPlayer = true;
	m_HurtPlayerTimer = 0.0f;

	m_DropAudio = snd::INVALID_SOUNDBUFFER;
	m_ClimbAudio = snd::INVALID_SOUNDBUFFER;
	m_DeathAudio = snd::INVALID_SOUNDBUFFER;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
HangingSpider::~HangingSpider()
{
	int i=0;

	for( i=0; i < MAX_HANGINGSPIDER_EMITTERS; ++i )
	{
		if( m_pEmitters[i] != 0 )
		{
			delete m_pEmitters[i];
			m_pEmitters[i] = 0;
		}
	}

	if( m_pJoints != 0 )
	{
		delete[] m_pJoints;
		m_pJoints = 0;
	}

	snd::RemoveSound( m_DropAudio );
	snd::RemoveSound( m_ClimbAudio );
	snd::RemoveSound( m_DeathAudio );
}

/////////////////////////////////////////////////////
/// Method: SetupHangingSpider
/// Params: [in]model
///
/////////////////////////////////////////////////////
void HangingSpider::SetupHangingSpider( mdl::ModelHGA* model )
{
	m_pModelData = model;

	m_Health = HANGINGSPIDER_HEALTH;

	// take a joint copy
	unsigned int i=0;
	m_JointCount = m_pModelData->GetNumBoneJoints();
	m_pJoints = new mdl::TBoneJoint[m_JointCount];
	for( i=0; i < m_JointCount; ++i )
	{
		m_pJoints[i] = m_pModelData->GetBoneJointsPtr()[i];
	}

	// emitters
	const res::EmitterResourceStore* er = 0;
	renderer::Texture texLoad;

	for( i=0; i < MAX_HANGINGSPIDER_EMITTERS; ++i )
	{
		m_pEmitters[i] = new efx::Emitter;
		DBG_ASSERT( (m_pEmitters[i] != 0) );
	}

	er = res::GetEmitterResource( math::RandomNumber(2000, 2004) );
	DBG_ASSERT( er != 0 );

	m_pEmitters[EMITTER_DEATH]->Create( *er->block );
	m_pEmitters[EMITTER_DEATH]->SetPos( zeroVec );
	m_pEmitters[EMITTER_DEATH]->SetRot( zeroVec );

	m_pEmitters[EMITTER_DEATH]->Disable();

	m_pEmitters[EMITTER_DEATH]->SetTimeOn( -1 );
	m_pEmitters[EMITTER_DEATH]->SetTimeOff( -1 );

	// grab the sprite resource
	res::SetupTexturesOnEmitter( m_pEmitters[EMITTER_DEATH] );

	// audio
	const char* sr = 0;

	sr = res::GetSoundResource( 310 );
	m_DropAudio = snd::SoundLoad( sr, m_SndLoad );

	sr = res::GetSoundResource( 311 );
	m_ClimbAudio = snd::SoundLoad( sr, m_SndLoad );

	sr = res::GetSoundResource( 312 );
	m_DeathAudio = snd::SoundLoad( sr, m_SndLoad );
}

/////////////////////////////////////////////////////
/// Method: PreDrawSetup
/// Params: None
///
/////////////////////////////////////////////////////
void HangingSpider::PreDrawSetup( bool pauseFlag )
{
	if( !pauseFlag )
		UpdateJoints(m_LastDeltaTime);
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void HangingSpider::Draw( )
{
	int i=0;

	if( m_pModelData != 0 &&
		((m_State != HangingSpiderState_Dead) && (m_State != HangingSpiderState_InActive)) )
	{
		glPushMatrix();
			glTranslatef( m_Pos.X, m_Pos.Y, m_Pos.Z );
			glRotatef( m_RotationAngle, 0.0f, 1.0f, 0.0f );
			m_pModelData->Draw();
		glPopMatrix();
	}

	for( i=0; i < MAX_HANGINGSPIDER_EMITTERS; ++i )
		m_pEmitters[i]->Draw();
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void HangingSpider::DrawDebug()
{
#ifdef _DEBUG
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	glPushMatrix();
		glTranslatef( m_BoundSphere.vCenterPoint.X, m_BoundSphere.vCenterPoint.Y, m_BoundSphere.vCenterPoint.Z );
		renderer::DrawSphere( m_BoundSphere.fRadius );
	glPopMatrix();

#endif // _DEBUG
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void HangingSpider::Update( float deltaTime )
{
	unsigned int i=0;

	for( i=0; i < MAX_HANGINGSPIDER_EMITTERS; ++i )
		m_pEmitters[i]->Update( deltaTime );

	if( m_State == HangingSpiderState_InActive )
		return;

	if( m_State == HangingSpiderState_Dead )
	{
		m_BoundSphere.vCenterPoint = math::Vec3( m_Pos.X+m_pJoints[BOUNDING_JOINT_INDEX].final.m41, m_pJoints[BOUNDING_JOINT_INDEX].final.m42, m_Pos.Z+m_pJoints[BOUNDING_JOINT_INDEX].final.m43 );
		m_BoundSphere.vCenterPoint = math::RotateAroundPoint( m_BoundSphere.vCenterPoint, m_Pos, math::DegToRad(m_RotationAngle) );

		if( m_DeadTimer == DEAD_TIME )
		{
			m_pEmitters[EMITTER_DEATH]->SetPos( m_BoundSphere.vCenterPoint );
			m_pEmitters[EMITTER_DEATH]->Enable();

			m_pEmitters[EMITTER_DEATH]->Disable();

			if( m_DeathAudio != snd::INVALID_SOUNDBUFFER )
				AudioSystem::GetInstance()->PlayAudio( m_DeathAudio, m_Pos, false );
		}

		m_DeadTimer -= deltaTime;

		if( m_DeadTimer <= 0.0f )
		{
			m_DeadTimer = 0.0f;
			m_State = HangingSpiderState_InActive;
		}

		return;
	}

	m_LastDeltaTime = deltaTime;

	ProcessState();

	m_BoundSphere.vCenterPoint = math::Vec3( m_Pos.X+m_pJoints[BOUNDING_JOINT_INDEX].final.m41, m_pJoints[BOUNDING_JOINT_INDEX].final.m42, m_Pos.Z+m_pJoints[BOUNDING_JOINT_INDEX].final.m43 );
	m_BoundSphere.vCenterPoint = math::RotateAroundPoint( m_BoundSphere.vCenterPoint, m_Pos, math::DegToRad(m_RotationAngle) );

	if( m_HurtPlayerTimer > 0.0f )
	{
		m_HurtPlayerTimer -= deltaTime;
		if( m_HurtPlayerTimer <= 0.0f )
		{
			m_HurtPlayerTimer = 0.0f;
			m_HurtPlayer = true;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateJoints
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void HangingSpider::UpdateJoints( float deltaTime )
{
	unsigned int i = 0;
	mdl::TBoneJointKeyframe curFrame;
	mdl::TBoneJointKeyframe prevFrame;	

	math::Vec3 vPrimaryTranslate( 0.0f, 0.0f, 0.0f );
	math::Quaternion qPrimaryRotation( 0.0f, 0.0f, 0.0f, 1.0f );

	math::Vec3 vCurPos( 0.0f, 0.0f, 0.0f );
	math::Vec3 vPrevPos( 0.0f, 0.0f, 0.0f );
	math::Quaternion qPrev( 0.0f, 0.0f, 0.0f, 1.0f );
	math::Quaternion qCur( 0.0f, 0.0f, 0.0f, 1.0f );

	// setup the bone pose for this enemies animation
	if( m_pPrimaryAnim )
	{
		int nFrame = 0;
		int nStartFrame = 0;

		m_LocalPrimaryAnim.animTimer += (deltaTime*1000.0f);

		// find out current time from the animation timer
		m_LocalPrimaryAnim.dCurrentTime = m_LocalPrimaryAnim.animTimer;//m_LocalPrimaryAnim.animTimer.GetTimeInMilliseconds();

		// should the current time be offset from the start frame (anims usually start at frame 0 though)
		float fTimeOffset = (float)((( nStartFrame )*1000) / m_LocalPrimaryAnim.fAnimationFramerate );
		m_LocalPrimaryAnim.dCurrentTime += fTimeOffset; 

		if( (m_LocalPrimaryAnim.nTotalFrames > 0) && (m_LocalPrimaryAnim.nTotalFrames < m_LocalPrimaryAnim.nTotalFrames) )
			m_LocalPrimaryAnim.fEndTime = (float)( ( (m_LocalPrimaryAnim.nTotalFrames)*1000) / m_LocalPrimaryAnim.fAnimationFramerate );
		else 
			m_LocalPrimaryAnim.fEndTime = (float)m_LocalPrimaryAnim.dTotalAnimationTime;

		// get the animation status
		if( m_LocalPrimaryAnim.dCurrentTime > m_LocalPrimaryAnim.fEndTime )
		{
			// animation has finished, check for a polled animation
			if( m_pPolledAnim )
			{
				m_pPrimaryAnim = m_pPolledAnim;
				m_LocalPrimaryAnim = m_LocalPolledAnim;

				if( m_LocalPrimaryAnim.bLooping )
				{
					m_LocalPrimaryAnim.nCurrentFrame = 0;
					//m_LocalPrimaryAnim.animTimer.Reset();
					m_LocalPrimaryAnim.animTimer = 0.0f;
					m_LocalPrimaryAnim.dCurrentTime = fTimeOffset;
				}
				else
				{
					m_LocalPrimaryAnim.dCurrentTime = m_LocalPrimaryAnim.fEndTime; 
				}

				// clear out the poll animation
				m_pPolledAnim = 0;
			}
			else
			{
				// no animation waiting, either loop or stop
				//if( !m_Paused )
				{
					if( m_LocalPrimaryAnim.bLooping )
					{
						m_LocalPrimaryAnim.nCurrentFrame = 0;
						//m_LocalPrimaryAnim.animTimer.Reset();
						m_LocalPrimaryAnim.animTimer = 0.0f;
						m_LocalPrimaryAnim.dCurrentTime = fTimeOffset;
					}
					else
					{
						m_LocalPrimaryAnim.dCurrentTime = m_LocalPrimaryAnim.fEndTime; 
					}
				}
			}
		} 

		// go through the joints and transform/rotate
		for( i = 0; i < m_JointCount; i++ )
		{
			mdl::TBoneJoint* pJoint = &m_pJoints[i];

			// allow enough time to pass before updating the frame
			nFrame = m_LocalPrimaryAnim.nCurrentFrame;
			//if( !m_Paused )
			{
				while( (nFrame < m_LocalPrimaryAnim.nTotalFrames) && (m_pPrimaryAnim->pFrames[i][nFrame].fTime < m_LocalPrimaryAnim.dCurrentTime) )
				{
					nFrame++;
				}
			}
			m_LocalPrimaryAnim.nCurrentFrame = nFrame;

			if( nFrame == 0 )
			{
				// first frame
				vPrimaryTranslate = m_pPrimaryAnim->pFrames[i][0].vTranslate;
				qPrimaryRotation = m_pPrimaryAnim->pFrames[i][0].qRotation;
			}
			else if( nFrame == m_LocalPrimaryAnim.nTotalFrames )
			{
				// last frame
				vPrimaryTranslate = m_pPrimaryAnim->pFrames[i][nFrame-1].vTranslate;
				qPrimaryRotation = m_pPrimaryAnim->pFrames[i][nFrame-1].qRotation;
			}
			else
			{
				// between first and last frame 
				curFrame = m_pPrimaryAnim->pFrames[i][nFrame];
				prevFrame = m_pPrimaryAnim->pFrames[i][nFrame-1];

				// find a point between the previous and current frame to move to
				vCurPos = m_pPrimaryAnim->pFrames[i][nFrame].vTranslate;
				vPrevPos = m_pPrimaryAnim->pFrames[i][nFrame-1].vTranslate;

				vPrimaryTranslate = Lerp( vPrevPos, vCurPos, deltaTime );

				// use quaternions
				qPrev = m_pPrimaryAnim->pFrames[i][nFrame-1].qRotation;
				qCur = m_pPrimaryAnim->pFrames[i][nFrame].qRotation;

				qPrimaryRotation = Slerp( qPrev, qCur, deltaTime );
			}

			pJoint->relative = math::Matrix4x4( qPrimaryRotation );
			pJoint->relative.SetTranslation( vPrimaryTranslate );

			// if bone has a parent, multiply the child by the parent
			if ( pJoint->nParentJointIndex != -1 )
			{
				pJoint->absolute.SetMatrix( m_pJoints[pJoint->nParentJointIndex].absolute.m  );
				pJoint->absolute = pJoint->relative * pJoint->absolute;
			}
			else
				pJoint->absolute.SetMatrix( pJoint->relative.m  );

			// assign results to a final matrix to update the verts
			pJoint->final.SetMatrix( pJoint->absolute.m );
		}
	}
	else
	{
		// go through the joints and just set the bind pose
		for( i = 0; i < m_JointCount; i++ )
		{
			mdl::TBoneJoint* pJoint = &m_pJoints[i];

			// if bone has a parent, multiply the child by the parent
			if ( pJoint->nParentJointIndex != -1 )
			{
				pJoint->absolute.SetMatrix( m_pJoints[pJoint->nParentJointIndex].absolute.m  );
				pJoint->absolute = pJoint->relative * pJoint->absolute;
			}
			else
				pJoint->absolute.SetMatrix( pJoint->relative.m  );

			// assign results to a final matrix to update the verts
			pJoint->final.SetMatrix( pJoint->absolute.m );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateSkins
/// Params: None
///
/////////////////////////////////////////////////////
void HangingSpider::UpdateSkins()
{
	unsigned int i = 0, j = 0;

	for ( i = 0; i < m_pModelData->GetNumSkinMeshes(); i++ )
	{
		// update each skin with the current frame pose
		mdl::SkinMesh* pSkin = &m_pModelData->GetSkinMeshListPtr()[i];

		if( !pSkin->bDraw )
			continue;

		// update the mesh if it's enabled
		for ( j = 0; j < pSkin->nNumPoints; j++ )
		{
			int nJointIndex0 = pSkin->pVertexWeights[j][0].nBoneIndex;
			float fWeight0 = pSkin->pVertexWeights[j][0].fWeight;

			int nJointIndex1 = pSkin->pVertexWeights[j][1].nBoneIndex;
			float fWeight1 = pSkin->pVertexWeights[j][1].fWeight;

			int nJointIndex2 = pSkin->pVertexWeights[j][2].nBoneIndex;
			float fWeight2 = pSkin->pVertexWeights[j][2].fWeight;

			int nJointIndex3 = pSkin->pVertexWeights[j][3].nBoneIndex;
			float fWeight3 = pSkin->pVertexWeights[j][3].fWeight;

			// move each vertex by the joints final matrix
			// nJointIndex0 and nWeight0 should always be valid, as there is always at least one bone affecting the vertex
			pSkin->pVertices[j] = math::Vec3( 0.0f, 0.0f, 0.0f );
					
			math::Vec3 vInfluence = math::TransformByMatrix4x4( pSkin->pVertexWeights[j][0].vTransformedVertex, &m_pJoints[ nJointIndex0 ].final.m[0] );
			vInfluence *= fWeight0;
			pSkin->pVertices[j] += vInfluence;

			// ADD POSSIBLE WEIGHT 2
			if( fWeight1 > 0.0f && nJointIndex1 != -1 )
			{
				// move vert
				vInfluence = math::TransformByMatrix4x4( pSkin->pVertexWeights[j][1].vTransformedVertex, &m_pJoints[ nJointIndex1 ].final.m[0]  );
				vInfluence *= fWeight1;
				pSkin->pVertices[j] += vInfluence;
			}
			// ADD POSSIBLE WEIGHT 3
			if( fWeight2 > 0.0f && nJointIndex2 != -1 )
			{
				// move vert
				vInfluence = math::TransformByMatrix4x4( pSkin->pVertexWeights[j][2].vTransformedVertex, &m_pJoints[ nJointIndex2 ].final.m[0] );
				vInfluence *= fWeight2;
				pSkin->pVertices[j] += vInfluence;
			}
			// ADD POSSIBLE WEIGHT 4
			if( fWeight3 > 0.0f && nJointIndex3 != -1 )
			{
				// move vert
				vInfluence = math::TransformByMatrix4x4( pSkin->pVertexWeights[j][3].vTransformedVertex, &m_pJoints[ nJointIndex3 ].final.m[0]  );
				vInfluence *= fWeight3;
				pSkin->pVertices[j] += vInfluence;
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SetAnimation
/// Params: [in]nAnimId, [in]nFrameOffset, [in]bForceChange
///
/////////////////////////////////////////////////////
void HangingSpider::SetAnimation( unsigned int nAnimId, int nFrameOffset, bool bForceChange )
{
	if( m_pModelData != 0 )
	{
		unsigned int i=0;

		if( m_pPrimaryAnim )
		{
			// if it asks for the same animation and the animation is looping then there's no need to change
			if( ( m_pPrimaryAnim->nAnimId == nAnimId ) && (m_pPrimaryAnim->bLooping || bForceChange==false) )
			{
				return;
			}
		}

		// forcing a change, update the primary animation pointer
		if( bForceChange )
		{
			for( i=0; i < m_pModelData->GetBoneAnimationList().size(); ++i )
			{
				mdl::BoneAnimation* pBoneAnim = m_pModelData->GetBoneAnimationList()[i];

				if( pBoneAnim->nAnimId == nAnimId  )
				{
					m_pPrimaryAnim = pBoneAnim;

					// take a copy of the frame data
					m_LocalPrimaryAnim.nAnimId					= m_pPrimaryAnim->nAnimId;
					m_LocalPrimaryAnim.fAnimationFramerate		= m_pPrimaryAnim->fAnimationFramerate;
					m_LocalPrimaryAnim.dTotalAnimationTime		= m_pPrimaryAnim->dTotalAnimationTime;
					m_LocalPrimaryAnim.nTotalJoints				= m_pPrimaryAnim->nTotalJoints;
					m_LocalPrimaryAnim.nTotalFrames				= m_pPrimaryAnim->nTotalFrames;

					m_LocalPrimaryAnim.bLooping					= m_pPrimaryAnim->bLooping;
					m_LocalPrimaryAnim.nCurrentFrame			= m_pPrimaryAnim->nCurrentFrame;
					m_LocalPrimaryAnim.dCurrentTime				= m_pPrimaryAnim->dCurrentTime;
					m_LocalPrimaryAnim.fEndTime					= m_pPrimaryAnim->fEndTime;

					if( nFrameOffset != -1 && nFrameOffset < m_LocalPrimaryAnim.nTotalFrames )
						m_LocalPrimaryAnim.nCurrentFrame = nFrameOffset;
					else
						m_LocalPrimaryAnim.nCurrentFrame = 0;
					//m_LocalPrimaryAnim.animTimer.Reset();
					//m_LocalPrimaryAnim.animTimer.Start();
					m_LocalPrimaryAnim.animTimer = 0.0f;
					return;
				}
			}
		}
		else
		{
			// not forcing a change so enable the poll animation
 			for( i=0; i < m_pModelData->GetBoneAnimationList().size(); ++i )
			{
				mdl::BoneAnimation* pBoneAnim = m_pModelData->GetBoneAnimationList()[i];

				if( pBoneAnim->nAnimId == nAnimId )
				{
					m_pPolledAnim = pBoneAnim;

					// take a copy of the frame data
					m_LocalPolledAnim.nAnimId					= m_pPolledAnim->nAnimId;
					m_LocalPolledAnim.fAnimationFramerate		= m_pPolledAnim->fAnimationFramerate;
					m_LocalPolledAnim.dTotalAnimationTime		= m_pPolledAnim->dTotalAnimationTime;
					m_LocalPolledAnim.nTotalJoints				= m_pPolledAnim->nTotalJoints;
					m_LocalPolledAnim.nTotalFrames				= m_pPolledAnim->nTotalFrames;

					m_LocalPolledAnim.bLooping					= m_pPolledAnim->bLooping;
					m_LocalPolledAnim.nCurrentFrame				= m_pPolledAnim->nCurrentFrame;
					m_LocalPolledAnim.dCurrentTime				= m_pPolledAnim->dCurrentTime;
					m_LocalPolledAnim.fEndTime					= m_pPolledAnim->fEndTime;

					if( nFrameOffset != -1 && nFrameOffset < m_LocalPolledAnim.nTotalFrames )
						m_LocalPolledAnim.nCurrentFrame = nFrameOffset;
					else
						m_LocalPolledAnim.nCurrentFrame = 0;
					//m_LocalPolledAnim.animTimer.Reset();
					//m_LocalPolledAnim.animTimer.Start();
					return;
				}
			}
		}

		// got this far, the animation doesn't exist so just set the bind pose
		m_pPrimaryAnim = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: ProcessState
/// Params: None
///
/////////////////////////////////////////////////////
void HangingSpider::ProcessState()
{
	switch( m_State )
	{
		case HangingSpiderState_InActive:
		break;

		case HangingSpiderState_IdleUp:
		{
			m_DropTimer -= m_LastDeltaTime;
			
			if( m_DropTimer <= 0.0f )
			{
				m_DropTimer = 0.0f;

				SetAnimation( HangingSpiderAnim::DROP, -1, true );
				m_State = HangingSpider::HangingSpiderState_Drop;

				if( m_DropAudio != snd::INVALID_SOUNDBUFFER )
					AudioSystem::GetInstance()->PlayAudio( m_DropAudio, m_Pos, false );
			}

		}break;
		case HangingSpiderState_IdleDown:
		{
			m_ResetTimer -= m_LastDeltaTime;
			
			if( m_ResetTimer <= 0.0f )
			{
				SetAnimation( HangingSpiderAnim::CLIMB, -1, true );
				m_State = HangingSpider::HangingSpiderState_Climb;

				if( m_ClimbAudio != snd::INVALID_SOUNDBUFFER )
					AudioSystem::GetInstance()->PlayAudio( m_ClimbAudio, m_Pos, false );
			}
		}break;
		case HangingSpiderState_Drop:
		{
			if( m_LocalPrimaryAnim.nCurrentFrame == m_LocalPrimaryAnim.nTotalFrames )
			{
				SetAnimation( HangingSpiderAnim::SWING, -1, true );
				m_State = HangingSpiderState_Swing;
			}

		}break;
		case HangingSpiderState_Swing:
		{
			if( m_LocalPrimaryAnim.nCurrentFrame == m_LocalPrimaryAnim.nTotalFrames )
			{
				m_CompleteSwings++;

				if( m_CompleteSwings < m_NumSwings )
					SetAnimation( HangingSpiderAnim::SWING, -1, true );
				else
				{
					SetAnimation( HangingSpiderAnim::IDLE_DOWN, -1, true );
					m_State = HangingSpider::HangingSpiderState_IdleDown;
				}
			}
		}break;
		case HangingSpiderState_Climb:
		{
			if( m_LocalPrimaryAnim.nCurrentFrame == m_LocalPrimaryAnim.nTotalFrames )
			{
				SetAnimation( HangingSpiderAnim::IDLE_UP, -1, true );

				// reset the drop timer
				m_DropTimer = math::RandomNumber( DROP_TIME_MIN, DROP_TIME_MAX );
				m_ResetTimer = math::RandomNumber( RESET_TIME_MIN, RESET_TIME_MAX );

				m_NumSwings = math::RandomNumber( SWING_MIN, SWING_MAX );
				m_CompleteSwings = 0;

				m_State = HangingSpider::HangingSpiderState_IdleUp;
			}

		}break;

		case HangingSpiderState_Dead:
		break;

		default:
			DBG_ASSERT_MSG( 0, "unknown enemy state" );
			break;
	}
}

/////////////////////////////////////////////////////
/// Method: SpawnBlood
/// Params: None
///
/////////////////////////////////////////////////////
void HangingSpider::SpawnBlood()
{
	m_pEmitters[EMITTER_DEATH]->SetPos( m_BoundSphere.vCenterPoint );
	m_pEmitters[EMITTER_DEATH]->Enable();

	m_pEmitters[EMITTER_DEATH]->Disable();
}
