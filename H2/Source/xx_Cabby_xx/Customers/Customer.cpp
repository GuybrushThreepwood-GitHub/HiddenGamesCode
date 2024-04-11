
/*===================================================================
	File: Customer.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "CabbyConsts.h"
#include "Cabby.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"
#include "Level/Level.h"

#include "Physics/PhysicsWorld.h"
#include "Customers/Customer.h"
#include "Customers/CustomerAnimationLookup.h"

namespace
{
	const float ROTATION_INCREMENT = 360.0f;
	const float ANGLE_TOLERANCE = 10.0f;

	const float CULLING_RADIUS = 1.4f;

	const float ANGRY_TIME	=	1.0f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Customer::Customer()
{
	m_pModelData = 0;
	m_pPrimaryAnim = 0;
	m_pPolledAnim = 0;

	m_JointCount = 0;
	m_pJoints = 0;

	m_Type = CustomerType_Unknown;
	m_State = CustomerState_InActive;
	m_DrawState = true;
	m_AlphaUpdate = 0.0f;
	m_CurrentAlpha = 0;
	m_WaitTime = 0.0f;
	m_TravelTime = 0.0f;
	m_CargoTravelTimer = false;
	m_CheckWatchTimer = 0.0f;

	m_Pos = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_DropOffWalkPoint = math::Vec3( 0.0f, 0.0f, 0.0f );

	m_IsFat = false;
	m_BodyModel = -1;
	m_TextureCharacter = renderer::INVALID_OBJECT;
	m_TextureParcel = renderer::INVALID_OBJECT;

	m_IsAngry = false;
	m_AngryTime = 0.0f;

	m_HasCargo = false;
	m_IsCargoCustomer = false;

	m_BoundSphere.vCenterPoint = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_BoundSphere.fRadius = CULLING_RADIUS;

	m_UnProjectedPoint = math::Vec2( 0.0f, 0.0f );

	m_TargetRotation = 0.0f;
	m_RotationAngle = 0.0f;

	m_SpawnedZone = -1;
	m_SpawnNumeric = -1;
	m_DestinationZone = -1;
	m_DestinationNumeric = -1;

	m_LastDeltaTime = 0.0f;

	m_GameData = GetScriptDataHolder()->GetGameData();
	m_FareValue = m_GameData.FARE_LOW;
	m_PerfectFare = true;

	m_VoicePitchTweak = 0.0f;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Customer::~Customer()
{
	if( m_pJoints != 0 )
	{
		delete[] m_pJoints;
		m_pJoints = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: SetupCustomer
/// Params: [in]type
///
/////////////////////////////////////////////////////
void Customer::SetupCustomer( CustomerType type, bool hasCargo, mdl::ModelHGA* model )
{
	unsigned int i=0;
	m_Type = type;
	m_HasCargo = hasCargo;
	m_pModelData = model;

	// take a joint copy
	m_JointCount = m_pModelData->GetNumBoneJoints();
	m_pJoints = new mdl::TBoneJoint[m_JointCount];
	for( i=0; i < m_JointCount; ++i )
	{
		m_pJoints[i] = m_pModelData->GetBoneJointsPtr()[i];
	}

	if( m_HasCargo )
		m_IdleAnim = math::RandomNumber(CustomerAnim::IDLE_WITHPARCEL, CustomerAnim::WAVING_WITHPARCEL);
	else
		m_IdleAnim = math::RandomNumber( CustomerAnim::IDLE, CustomerAnim::WAVING2);

	switch( m_Type )
	{
		case CustomerType_Male:
		{
			m_IsFat = false;
			m_BodyModel = m_GameData.CUSTOMER_MALE_MESH;
			int tex = math::RandomNumber( m_GameData.CUSTOMER_MODEL_TEXTURE_START, m_GameData.CUSTOMER_MODEL_TEXTURE_END );
			m_TextureCharacter = res::GetTextureResource( tex )->texId;
			if( m_HasCargo )
				m_TextureParcel = res::GetTextureResource( math::RandomNumber( m_GameData.CUSTOMER_PARCEL_TEXTURE_START, m_GameData.CUSTOMER_PARCEL_TEXTURE_END ) )->texId;
			GetValue( tex );
			//m_FareValue = m_GameData.FARE_FAT;
			m_Gender = CUSTOMER_MALE;
		}break;
		case CustomerType_Female:
		{
			m_IsFat = false;
			m_BodyModel = m_GameData.CUSTOMER_FEMALE_MESH;
			int tex = math::RandomNumber( m_GameData.CUSTOMER_MODEL_TEXTURE_START, m_GameData.CUSTOMER_MODEL_TEXTURE_END );
			m_TextureCharacter = res::GetTextureResource( tex )->texId;
			if( m_HasCargo )
				m_TextureParcel = res::GetTextureResource( math::RandomNumber( m_GameData.CUSTOMER_PARCEL_TEXTURE_START, m_GameData.CUSTOMER_PARCEL_TEXTURE_END ) )->texId;
			GetValue( tex );
			//m_FareValue = m_GameData.FARE_FAT;
			m_Gender = CUSTOMER_FEMALE;
		}break;

		default:
			DBG_ASSERT(0); // Invalid customer
			break;
	}
}

/////////////////////////////////////////////////////
/// Method: SetupSubmeshes
/// Params: None
///
/////////////////////////////////////////////////////
void Customer::SetupSubmeshes()
{
	m_pModelData->SetSkinDrawState( -1, false );

	m_pModelData->SetSkinDrawState( m_BodyModel, true );

	if( m_HasCargo )
		m_pModelData->SetSkinDrawState( m_GameData.CUSTOMER_PARCEL_MESH, false );
}

/////////////////////////////////////////////////////
/// Method: SetAngry
/// Params: None
///
/////////////////////////////////////////////////////
void Customer::SetAngry()
{
	m_IsAngry = true;
	m_AngryTime = m_GameData.ANGRY_TIME;
}

/////////////////////////////////////////////////////
/// Method: PreDrawSetup
/// Params: None
///
/////////////////////////////////////////////////////
void Customer::PreDrawSetup()
{
	UpdateJoints(m_LastDeltaTime);
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void Customer::Draw( bool pauseFlag )
{
	if( m_pModelData != 0 )
	{
		if( m_State == CustomerState_Waiting ||
			m_State == CustomerState_WalkToTaxi ||
			m_State == CustomerState_DroppedOff || 
			
			m_State == CustomerState_WaitingWithCargo ||
			m_State == CustomerState_WaitingForCargo ||
			m_State == CustomerState_WalkToTaxiWithCargo ||
			m_State == CustomerState_WalkToTaxiForCargo ||
			m_State == CustomerState_WalkAway ||
			m_State == CustomerState_WalkAwayWithCargo )
		{
			//if( m_CurrentAlpha > 0 )
			{
				if( m_BodyModel != -1 )
				{
					m_pModelData->ChangeTextureOnMaterialId( m_BodyModel, 0, m_TextureCharacter );
					m_pModelData->SetSkinDrawState( m_BodyModel, m_DrawState );
				}

				if( m_HasCargo )
				{
					m_pModelData->ChangeTextureOnMaterialId( static_cast<unsigned int>( m_GameData.CUSTOMER_PARCEL_MESH ), 0, m_TextureParcel);
					m_pModelData->SetSkinDrawState( static_cast<unsigned int>( m_GameData.CUSTOMER_PARCEL_MESH ), m_DrawState );
				}
			}

			//renderer::OpenGL::GetInstance()->SetColour4ub( 225, 225, 225, m_CurrentAlpha );

			glPushMatrix();
				glTranslatef( m_Pos.X, m_Pos.Y, m_Pos.Z );
				glRotatef( m_RotationAngle, 0.0f, 1.0f, 0.0f );

				m_pModelData->Draw();
			glPopMatrix();
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Customer::Update( float deltaTime )
{
	unsigned int i=0;

	m_LastDeltaTime = deltaTime;

	// get screen space position of customer	
	GLfloat *mdl = renderer::OpenGL::GetInstance()->GetModelViewMatrix();
	GLfloat *proj = renderer::OpenGL::GetInstance()->GetProjectionMatrix();
	GLint *viewport = renderer::OpenGL::GetInstance()->GetViewport();

	gluUtil::gluProjectf( m_Pos.X, m_Pos.Y, m_Pos.Z,
				mdl, proj, viewport,
				&m_UnProjectedPoint.X, &m_UnProjectedPoint.Y, 0 );

	for( i = 0; i < m_pModelData->GetNumSkinMeshes(); i++ )
		m_pModelData->SetSkinDrawState( i, false );

	m_BoundSphere.vCenterPoint = m_Pos;
	m_BoundSphere.vCenterPoint.Y = (m_Pos.Y+CULLING_RADIUS);

	if( m_RotationAngle != m_TargetRotation )
	{
		if( m_TargetRotation < m_RotationAngle )
			m_RotationAngle -= ROTATION_INCREMENT*deltaTime;
		else if( m_TargetRotation > m_RotationAngle )
			m_RotationAngle += ROTATION_INCREMENT*deltaTime;

		if( (m_RotationAngle-m_TargetRotation) < ANGLE_TOLERANCE &&
			(m_RotationAngle-m_TargetRotation) > -ANGLE_TOLERANCE )
				m_RotationAngle = m_TargetRotation;
	}

	// show angry bubble
	if( m_IsAngry )
	{
		m_AngryTime -= deltaTime;
		if( m_AngryTime < 0.0f )
			m_IsAngry = false;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateTimer
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Customer::UpdateTimer( float deltaTime )
{
	if( m_State == CustomerState_Waiting ||
		m_State == CustomerState_WaitingWithCargo )
	{
		m_WaitTime += deltaTime;

		if( m_WaitTime >= static_cast<float>(m_GameData.WAIT_DECREASE_SECS) )
		{
			m_FareValue -= m_GameData.WAIT_FARE_LOSS;

			// reset
			m_WaitTime = 0.0f;

			if( m_FareValue < m_GameData.FARE_MINIMUM )
				m_FareValue = m_GameData.FARE_MINIMUM;

			m_PerfectFare = false;
		}
	}

	if( m_State == CustomerState_InTaxi ||
		( m_State == CustomerState_WaitingForCargo && m_CargoTravelTimer) )
	{
		m_TravelTime += deltaTime;

		if( m_TravelTime >= static_cast<float>(m_GameData.TRAVEL_DECREASE_SECS) )
		{
			m_FareValue -= m_GameData.TRAVEL_FARE_LOSS;

			// reset
			m_TravelTime = 0.0f;

			if( m_FareValue < m_GameData.FARE_MINIMUM )
				m_FareValue = m_GameData.FARE_MINIMUM;

			m_PerfectFare = false;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateSkins
/// Params: None
///
/////////////////////////////////////////////////////
void Customer::UpdateSkins()
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
/// Method: UpdateJoints
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Customer::UpdateJoints( float deltaTime )
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
		m_LocalPrimaryAnim.dCurrentTime = m_LocalPrimaryAnim.animTimer;

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
/// Method: UpdateFade
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Customer::UpdateFade( float deltaTime )
{
	if( m_State == CustomerState_Waiting ||
		m_State == CustomerState_WalkToTaxi ||
		m_State == CustomerState_InTaxi ||
		
		m_State == CustomerState_WaitingWithCargo ||
		m_State == CustomerState_WaitingForCargo ||
		m_State == CustomerState_WalkToTaxiWithCargo ||
		m_State == CustomerState_WalkToTaxiForCargo )
	{
		if( m_AlphaUpdate < 255 )
			m_AlphaUpdate = (196.0f*deltaTime);
		else
			m_AlphaUpdate = 0.0f;

		m_CurrentAlpha += static_cast<int>(m_AlphaUpdate);
	}
	else if( m_State == CustomerState_DroppedOff ||
			m_State == CustomerState_WalkAway ||
			m_State == CustomerState_WalkAwayWithCargo )
	{
		if( m_CurrentAlpha > 0 )
		{
			m_AlphaUpdate = (128.0f*deltaTime);
		}
		else
			m_AlphaUpdate = 0.0f;

		m_CurrentAlpha -= static_cast<int>(m_AlphaUpdate);
	}

	if( m_AlphaUpdate < 0.0f )
		m_AlphaUpdate = 0.0f;
	else if( m_AlphaUpdate > 255.0f )
		m_AlphaUpdate = 255.0f;

	if( m_CurrentAlpha < 0 )
	{
		m_CurrentAlpha = 0;
	}
	else if( m_CurrentAlpha > 255 )
	{
		m_CurrentAlpha = 255;
	}
}

/////////////////////////////////////////////////////
/// Method: SetIdleAnim
/// Params: None
///
/////////////////////////////////////////////////////
void Customer::SetIdleAnim()
{
	SetAnimation( m_IdleAnim, -1, true );
}

/////////////////////////////////////////////////////
/// Method: SetAnimation
/// Params: [in]nAnimId, [in]nFrameOffset, [in]bForceChange
///
/////////////////////////////////////////////////////
void Customer::SetAnimation( unsigned int nAnimId, int nFrameOffset, bool bForceChange )
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
/// Method: GetValue
/// Params: [in]texture
///
/////////////////////////////////////////////////////
void Customer::GetValue( int texture )
{
	switch( texture )
	{
		case CustomerValue_MaleHigh:
		case CustomerValue_FemaleHigh:
			m_FareValue = m_GameData.FARE_HIGH;
		break;
		
		case CustomerValue_MaleMedium:
		case CustomerValue_FemaleMedium:
			m_FareValue = m_GameData.FARE_MEDIUM;
		break;	

		case CustomerValue_MaleLow:
		case CustomerValue_FemaleLow:
			m_FareValue = m_GameData.FARE_LOW;
		break;
		
		default:
			DBG_ASSERT(0);
		break;
	}
}
