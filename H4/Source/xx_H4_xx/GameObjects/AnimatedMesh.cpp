
/*===================================================================
	File: AnimatedMesh.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "CollisionBase.h"
#include "RenderBase.h"
#include "ModelBase.h"

#include "Level/LevelTypes.h"
#include "Physics/PhysicsContact.h"

#include "H4.h"

#include "Resources/AnimatedResources.h"
#include "Resources/TextureResources.h"

#include "AnimatedMesh.h"

namespace
{
	const int SKIN_START = 0;
	const int SKIN_END = 35;

	const int SKIN_HI_START = 100;
	const int SKIN_HI_END = 135;

	const float COCOONED_TIMER = 10.0f;

	const int COCOONED_SKIN_START = 0;
	const int COCOONED_SKIN_END = 5;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
AnimatedMesh::AnimatedMesh( int objectId, math::Vec3& rot, int meshResourceIndex, lua_State* state )
	: BaseObject( PHYSICSBASICID_UNKNOWN, PHYSICSBASICID_UNKNOWN, objectId, -1, state )
	, m_LuaState(state)
	, m_Rot(rot)
	, m_pModelData(0)
	, m_MeshResourceIndex(meshResourceIndex)
	, m_pPrimaryAnim(0)
{
	std::memset( m_FunctionBuffer, 0, sizeof(char)*LUAFUNC_STRBUFFER );

	m_pModelData = res::LoadAnimatedModel( m_MeshResourceIndex );
	DBG_ASSERT_MSG( (m_pModelData != 0), "Could not load or find animated mesh model index was '%d'", meshResourceIndex );

	// take a joint copy
	unsigned int i=0;
	m_JointCount = m_pModelData->GetNumBoneJoints();
	m_pJoints = new mdl::TBoneJoint[m_JointCount];
	for( i=0; i < m_JointCount; ++i )
	{
		m_pJoints[i] = m_pModelData->GetBoneJointsPtr()[i];
	}

	SetAnimation( 0 );

	// random turbine anim
	if( m_MeshResourceIndex == 0 )
	{
		SetAnimation( math::RandomNumber( 0, 2 ) );
	}

	// HACK: hanged prisoners, random skin index
	if( m_MeshResourceIndex == 2 ||
		m_MeshResourceIndex == 3 ||
		m_MeshResourceIndex == 4 )
	{
		if( H4::GetHiResMode() )
		{
			const res::TextureResourceStore* rs = 0; 
			int index = math::RandomNumber( SKIN_HI_START, SKIN_HI_END );
			rs = res::GetTextureResource( index );
			DBG_ASSERT( rs != 0 );

			if( !rs->preLoad )
			{
				res::LoadTexture( index );
			}

			m_SkinIndex =  rs->texId;

			rs = res::GetTextureResource( 302 );
			DBG_ASSERT( rs != 0 );
			m_RopeIndex = rs->texId;
		}
		else
			m_SkinIndex = math::RandomNumber( SKIN_START, SKIN_END );
	}

	// silk random wriggle
	if( m_MeshResourceIndex == 5 ||
		m_MeshResourceIndex == 6 )
	{
		m_SkinIndex = math::RandomNumber( COCOONED_SKIN_START, COCOONED_SKIN_END );
		m_Timer = math::RandomNumber( 1.0f, COCOONED_TIMER );
	}

	m_CurrentTime = 0.0f;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
AnimatedMesh::~AnimatedMesh()
{
	if( m_MeshResourceIndex == 2 ||
		m_MeshResourceIndex == 3 ||
		m_MeshResourceIndex == 4 )
	{
		if( H4::GetHiResMode() )
		{
			renderer::RemoveTexture( m_SkinIndex );
		}
	}

	if( m_pJoints != 0 )
	{
		delete[] m_pJoints;
		m_pJoints = 0;
	}

	res::RemoveAnimatedModel(m_pModelData);
	m_pModelData = 0;
}

/////////////////////////////////////////////////////
/// Method: PostConstruction
/// Params: None
///
/////////////////////////////////////////////////////
int AnimatedMesh::PostConstruction()
{
	const math::Vec3& pos = GetPos();

	m_Sphere = m_pModelData->modelSphere;
	m_Sphere.vCenterPoint = pos;

	return(0);
}

/////////////////////////////////////////////////////
/// Method: UpdateJoints
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void AnimatedMesh::UpdateJoints( float deltaTime )
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
void AnimatedMesh::UpdateSkins()
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
/// Params: [in]nAnimId, [in]nFrameOffset
///
/////////////////////////////////////////////////////
void AnimatedMesh::SetAnimation( unsigned int nAnimId, int nFrameOffset )
{
	if( m_pModelData != 0 )
	{
		unsigned int i=0;

		if( m_pPrimaryAnim )
		{
			// if it asks for the same animation and the animation is looping then there's no need to change
			if( ( m_pPrimaryAnim->nAnimId == nAnimId ) && (m_pPrimaryAnim->bLooping) )
			{
				return;
			}
		}

		// update the primary animation pointer
		//if( bForceChange )
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

		// got this far, the animation doesn't exist so just set the bind pose
		m_pPrimaryAnim = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void AnimatedMesh::Update( float deltaTime )
{
//	if( m_pModelData != 0 )
//		m_pModelData->Update( deltaTime );

	// silk wrap
	if( m_MeshResourceIndex == 5 ||
		m_MeshResourceIndex == 6 )
	{
		if( m_LocalPrimaryAnim.nAnimId == 1 )
		{
			if( m_LocalPrimaryAnim.nCurrentFrame >= m_LocalPrimaryAnim.nTotalFrames )
				SetAnimation(0);
		}
		else
		{
			m_CurrentTime += deltaTime;

			if( m_CurrentTime >= m_Timer )
			{
				m_CurrentTime = 0.0f;
				m_Timer = math::RandomNumber( 1.0f, COCOONED_TIMER );

				SetAnimation(1);
			}
		}
	}

	UpdateJoints( deltaTime );
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void AnimatedMesh::Draw()
{
	//renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	//glPushMatrix();
	//	glTranslatef( m_Sphere.vCenterPoint.X,m_Sphere.vCenterPoint.Y,m_Sphere.vCenterPoint.Z );
	//	renderer::DrawSphere(m_Sphere.fRadius);
	//glPopMatrix();

	// HACK: hanged prisoners/drowned, random skin index
	if( m_MeshResourceIndex == 2 )
	{
		if( H4::GetHiResMode() )
		{
			m_pModelData->ChangeTextureOnMaterialId(0, 0, m_RopeIndex );
			m_pModelData->ChangeTextureOnMaterialId( 1, 0, m_SkinIndex );
		}
		else
			m_pModelData->ChangeTextureOnMaterial( 0, 0, m_SkinIndex );
	}
	else
	if(
		(m_MeshResourceIndex == 3 ||
		m_MeshResourceIndex == 4) )
	{
		if( H4::GetHiResMode() )
		{
			m_pModelData->ChangeTextureOnMaterialId( 0, 0, m_SkinIndex );
		}
		else
			m_pModelData->ChangeTextureOnMaterial( 0, 0, m_SkinIndex );
	}

	// silk wrapped
	if( m_MeshResourceIndex == 5 ||
		m_MeshResourceIndex == 6 )
		m_pModelData->ChangeTextureOnMaterial( 0, 0, m_SkinIndex );

	if( !renderer::OpenGL::GetInstance()->SphereInFrustum(m_Sphere.vCenterPoint.X,m_Sphere.vCenterPoint.Y,m_Sphere.vCenterPoint.Z, m_Sphere.fRadius) )
		return;

	const math::Vec3& pos = GetPos();

	renderer::OpenGL::GetInstance()->SetColour4ub( 128,128,128,255 );

	glPushMatrix();
		glTranslatef( pos.X, pos.Y, pos.Z );
		glRotatef( m_Rot.Y, 0.0f, 1.0f, 0.0f );
		glRotatef( m_Rot.X, 1.0f, 0.0f, 0.0f );
		glRotatef( m_Rot.Z, 0.0f, 0.0f, 1.0f );
		
		if( m_pModelData != 0 )
		{
			UpdateSkins();
			m_pModelData->Draw();
		}
	glPopMatrix();
}

