
/*===================================================================
	File: CrawlingSpider.cpp
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

#include "Resources/SpriteResources.h"
#include "Resources/EmitterResources.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"

#include "GameSystems.h"

#include "Enemy/CrawlingSpiderAnimationLookup.h"
#include "Enemy/CrawlingSpider.h"

namespace
{
	const float ROTATION_INCREMENT = 60.0f;
	const float ANGLE_TOLERANCE = 5.0f;

	const float CULLING_RADIUS = 0.4f;

	const float SEARCH_TIME_MIN = 1.0f;
	const float SEARCH_TIME_MAX = 2.0f;

	const float MAX_WALK_SPEED = 0.3f;

	const int CHASE_COUNT = 3;

	const float DEAD_FADE_TIME = 2.0f;
	const float DEAD_SINK_SPEED = 0.1f;
	const float DEAD_SINK_STOP = -1.0f;

	const int BOUNDING_JOINT_INDEX = 1;

	const float PLAYER_DAMAGE_DISTANCE = 2.0f;

	b2Vec2 zeroVec2(0.0f,0.0f);

#ifdef _DEBUG
	math::Vec3 vaPoints[256];
	int pointIndex = 0;
#endif // _DEBUG
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
CrawlingSpider::CrawlingSpider( NavigationMesh* pNavMesh )
	: PhysicsIdentifier( PHYSICSBASICID_CRAWLINGSPIDER, PHYSICSCASTID_CRAWLINGSPIDER ) 
	, NavObject( pNavMesh )
{
	int i=0;

	m_pModelData = 0;
	m_pPrimaryAnim = 0;
	m_pPolledAnim = 0;
	m_JointCount = 0;
	m_pJoints = 0;

	m_State = CrawlingSpiderState_InActive;
	m_DrawState = true;
	
	m_Pos = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_PrevPos = m_Pos;
	m_Movement = math::Vec3(0.0f,0.0f,0.0f);
	m_Dir = math::Vec3( 0.0f, 0.0f, 1.0f );

	m_BoundSphere.vCenterPoint = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_BoundSphere.fRadius = CULLING_RADIUS;

	m_UnProjectedPoint = math::Vec2( 0.0f, 0.0f );

	m_MaxSpeed = MAX_WALK_SPEED;
	m_TargetRotation = 0.0f;
	m_RotationAngle = 0.0f;

	m_LastDeltaTime = 0.0f;
	m_NewWaypoint = false;

	m_DeadFadeTimer = DEAD_FADE_TIME;
	m_DeadAlpha = 255;

	for( i=0; i < MAX_CRAWLINGSPIDER_EMITTERS; ++i )
		m_pEmitters[i] = 0;

	m_LightZoneIndex = -1;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
CrawlingSpider::~CrawlingSpider()
{
	int i=0;

	for( i=0; i < MAX_CRAWLINGSPIDER_EMITTERS; ++i )
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

	if( m_pBody != 0 )
	{
		if( physics::PhysicsWorldB2D::GetWorld() )
			physics::PhysicsWorldB2D::GetWorld()->DestroyBody(m_pBody);

		m_pBody = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: SetupCrawlingSpider
/// Params: [in]model, [in]smallGib, [in]largeGib, [in]skinIndex
///
/////////////////////////////////////////////////////
void CrawlingSpider::SetupCrawlingSpider( mdl::ModelHGA* model )
{
	m_pModelData = model;

	// take a joint copy
	unsigned int i=0;
	m_JointCount = m_pModelData->GetNumBoneJoints();
	m_pJoints = new mdl::TBoneJoint[m_JointCount];
	for( i=0; i < m_JointCount; ++i )
	{
		m_pJoints[i] = m_pModelData->GetBoneJointsPtr()[i];
	}

	b2BodyDef bd;
	bd.type = b2_dynamicBody;
	bd.gravityScale = 0.0f;
	bd.awake = true;
	bd.fixedRotation = true;
	bd.linearDamping = 50.0f;
	
	// setup physics
	m_pBody = physics::PhysicsWorldB2D::GetWorld()->CreateBody(&bd);
	DBG_ASSERT_MSG( (m_pBody != 0), "Could not create Crawling Spider physics body" );

	b2CircleShape circleShape;
	b2FixtureDef fd;

	fd.friction = 0.5f;
	fd.restitution = 0.2f;
	fd.density = 50.0f;

	fd.filter.categoryBits = ENEMY_CATEGORY;
	fd.filter.maskBits = WORLDHIGH_CATEGORY | WORLDLOW_CATEGORY | ENEMY_CATEGORY | PLAYER_CATEGORY;
	fd.filter.groupIndex = ENEMY_GROUP;

	circleShape.m_p = b2Vec2( 0.0f, 0.0f );
	circleShape.m_radius = 0.45f;

	fd.shape = &circleShape;

	b2Fixture* pFixture = 0;
	pFixture = m_pBody->CreateFixture( &fd );
	pFixture->SetUserData( reinterpret_cast<void *>(this) );

	// mass setup
	b2MassData massData; 
	massData.mass = 0.1f;
	massData.center = b2Vec2( 0.0f, 0.0f );
	massData.I = 1.0f;
	
	m_pBody->SetMassData( &massData );

	m_pBody->SetUserData( reinterpret_cast<void *>(this) );

	// ray data
	m_CanSeePlayer = false;
	m_RayContact.SetZero();
	m_RayNormal.SetZero();

	m_SearchTime = math::RandomNumber( SEARCH_TIME_MIN, SEARCH_TIME_MAX );
	m_CurrentSearchTime = 0.0f;

	m_ContactWithPlayer = false;

	m_NavMeshEnable = false;

	// emitters
	const res::EmitterResourceStore* er = 0;
	renderer::Texture texLoad;

	for( i=0; i < MAX_CRAWLINGSPIDER_EMITTERS; ++i )
	{
		er = res::GetEmitterResource( math::RandomNumber(2000,2004) );
		DBG_ASSERT( er != 0 );

		m_pEmitters[i] = new efx::Emitter;
		DBG_ASSERT( (m_pEmitters[i] != 0) );

		m_pEmitters[i]->Create( *er->block );
		math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );
		m_pEmitters[i]->SetPos( zeroVec );
		m_pEmitters[i]->SetRot( zeroVec );

		m_pEmitters[i]->Disable();

		m_pEmitters[i]->SetTimeOn( -1 );
		m_pEmitters[i]->SetTimeOff( -1 );

		// grab the sprite resource
		res::SetupTexturesOnEmitter( m_pEmitters[i] );
	}
}

/////////////////////////////////////////////////////
/// Method: PreDrawSetup
/// Params: None
///
/////////////////////////////////////////////////////
void CrawlingSpider::PreDrawSetup( bool pauseFlag )
{
	if( !pauseFlag )
		UpdateJoints(m_LastDeltaTime);
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void CrawlingSpider::Draw( )
{
	int i=0;

	if( m_pModelData != 0 )
	{
		glPushMatrix();
			glTranslatef( m_Pos.X, m_Pos.Y, m_Pos.Z );
			glRotatef( m_RotationAngle, 0.0f, 1.0f, 0.0f );
			m_pModelData->Draw();
		glPopMatrix();
	}

	for( i=0; i < MAX_CRAWLINGSPIDER_EMITTERS; ++i )
		m_pEmitters[i]->Draw();
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void CrawlingSpider::DrawDebug()
{
#ifdef _DEBUG
	if( m_CanSeePlayer )
		renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,0,255 );
	else
		renderer::OpenGL::GetInstance()->SetColour4ub( 0,0,255,255 );

	glPushMatrix();
		glTranslatef( m_RayContact.x, 0.0f, m_RayContact.y );
		renderer::DrawSphere( 0.5f );
	glPopMatrix();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	glPushMatrix();
		glTranslatef( m_BoundSphere.vCenterPoint.X, m_BoundSphere.vCenterPoint.Y, m_BoundSphere.vCenterPoint.Z );
		renderer::DrawSphere( m_BoundSphere.fRadius );
	glPopMatrix();

	if (m_PathActive)
	{
		renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );

		pointIndex = 0;

		glPushMatrix();
			glTranslatef(0.0f, 0.0f, 0.0f); 
			
			// For line drawing, add 0.1 to all y values to make sure 
			// the lines are visible above the Navigation Mesh
			//
			// connect the waypoins to draw lines in blue
			NavigationPath::WAYPOINT_LIST::const_iterator iter = m_Path.WaypointList().begin();
			if (iter != m_Path.WaypointList().end())
			{
				NavigationPath::WAYPOINT LastWaypoint = *iter;
				++iter;
				for (;iter != m_Path.WaypointList().end();++iter)
				{
					const NavigationPath::WAYPOINT& waypoint= *iter;

					vaPoints[pointIndex] = LastWaypoint.Position;
					vaPoints[pointIndex+1] = waypoint.Position;

					pointIndex += 2;

					LastWaypoint = waypoint;
				}
			}
			renderer::OpenGL::GetInstance()->SetColour4ub(0, 0, 255, 255);
			glVertexPointer( 3, GL_FLOAT, sizeof(math::Vec3), vaPoints );
			glDrawArrays( GL_LINES, 0, pointIndex ); 

			// draw our current heading in red
			pointIndex = 0;
			const NavigationPath::WAYPOINT& waypoint= *m_NextWaypoint;
			
			vaPoints[pointIndex] = m_Pos;
			vaPoints[pointIndex+1] = waypoint.Position;
			
			pointIndex += 2;

			renderer::OpenGL::GetInstance()->SetColour4ub(255, 0, 0, 255);
			glVertexPointer( 3, GL_FLOAT, sizeof(math::Vec3), vaPoints );
			glDrawArrays( GL_LINES, 0, pointIndex ); 
		glPopMatrix();
		
		renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	}
#endif // _DEBUG
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void CrawlingSpider::Update( float deltaTime )
{
	unsigned int i=0;

	for( i=0; i < MAX_CRAWLINGSPIDER_EMITTERS; ++i )
		m_pEmitters[i]->Update( deltaTime );

	if( m_State == CrawlingSpiderState_InActive )
		return;

	if( m_State == CrawlingSpiderState_Dead )
	{
		if( m_pBody != 0 )
		{
			physics::PhysicsWorldB2D::GetWorld()->DestroyBody(m_pBody);
			m_pBody = 0;
		}

		m_BoundSphere.vCenterPoint = math::Vec3( m_Pos.X+m_pJoints[BOUNDING_JOINT_INDEX].final.m41, m_Pos.Y+m_pJoints[BOUNDING_JOINT_INDEX].final.m42, m_Pos.Z+m_pJoints[BOUNDING_JOINT_INDEX].final.m43 );
		m_BoundSphere.vCenterPoint = math::RotateAroundPoint( m_BoundSphere.vCenterPoint, m_Pos, math::DegToRad(m_RotationAngle) );

		if( m_DeadFadeTimer == DEAD_FADE_TIME )
		{	
			m_pEmitters[0]->SetPos( m_BoundSphere.vCenterPoint );
			m_pEmitters[0]->Enable();

			m_pEmitters[0]->Disable();
		}

		m_DeadFadeTimer -= deltaTime;
		m_DeadAlpha -= 1;

		if( m_DeadFadeTimer <= 0.0f )
		{
			m_DeadFadeTimer = 0.0f;

			m_Pos.Y -= DEAD_SINK_SPEED*deltaTime;
			if( m_Pos.Y < DEAD_SINK_STOP )
				m_State = CrawlingSpiderState_InActive;
		}
		if( m_DeadAlpha < 0 )
			m_DeadAlpha = 0;

		return;
	}

	m_LastDeltaTime = deltaTime;

	ProcessState();

	if( m_NavMeshEnable )
		UpdateNavMesh( deltaTime );
	else if( m_CanSeePlayer )
		UpdateDirect( deltaTime );
	else
		UpdateIdle( deltaTime );


	m_BoundSphere.vCenterPoint = math::Vec3( m_Pos.X+m_pJoints[BOUNDING_JOINT_INDEX].final.m41, m_Pos.Y+m_pJoints[BOUNDING_JOINT_INDEX].final.m42, m_Pos.Z+m_pJoints[BOUNDING_JOINT_INDEX].final.m43 );
	m_BoundSphere.vCenterPoint = math::RotateAroundPoint( m_BoundSphere.vCenterPoint, m_Pos, math::DegToRad(m_RotationAngle) );

	if( m_RotationAngle != m_TargetRotation )
	{
		if( m_TargetRotation < m_RotationAngle )
			m_RotationAngle -= ROTATION_INCREMENT*deltaTime;
		else if( m_TargetRotation > m_RotationAngle )
			m_RotationAngle += ROTATION_INCREMENT*deltaTime;

		if( std::fabs(m_RotationAngle-m_TargetRotation) < ANGLE_TOLERANCE )
				m_RotationAngle = m_TargetRotation;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateJoints
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void CrawlingSpider::UpdateJoints( float deltaTime )
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
void CrawlingSpider::UpdateSkins()
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
void CrawlingSpider::SetAnimation( unsigned int nAnimId, int nFrameOffset, bool bForceChange )
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
/// Method: HandleContact
/// Params: [in]idNum, [in]castId, [in]point, [in]pOtherShape, [in]pOtherBody
///
/////////////////////////////////////////////////////
void CrawlingSpider::HandleContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody )
{
	//int numeric1 = -9999;
	//int numeric2 = -9999;
	//int numeric3 = -9999;
	//int numeric4 = -9999;
	//int numeric5 = -9999;

	switch( idNum )
	{
		case PHYSICSBASICID_WORLD:
		{
			physics::PhysicsIdentifier* pUserData = 0;
			if( pOtherBody != 0 )
			{
				pUserData = reinterpret_cast<physics::PhysicsIdentifier*>( pOtherFixture->GetUserData() );
				DBG_ASSERT_MSG( (pUserData != 0), "Could not get physics user data on Crawling Spider world contact" );
				
				//numeric1 = pUserData->GetNumeric1();
				//numeric2 = pUserData->GetNumeric2();
				//numeric3 = pUserData->GetNumeric3();
				//numeric4 = pUserData->GetNumeric4();
				//numeric5 = pUserData->GetNumeric5();
			}

		}break;

		case PHYSICSBASICID_BULLET:
		{

		}break;

		case PHYSICSBASICID_PLAYER:
		{
			m_PathActive = false;

			if( m_State != CrawlingSpiderState_Dead )
			{
				// attack the player
				m_ContactWithPlayer = true;

				if( m_State != CrawlingSpiderState_Attack )
				{
					m_State = CrawlingSpiderState_Attack; 
				}
			}

		}break;

		case PHYSICSBASICID_ENEMY:
		case PHYSICSBASICID_FLOATINGHEAD:
		case PHYSICSBASICID_CRAWLINGSPIDER:
		{

		}break;

		default:
			// colliding with something that doesn't have an id
			DBG_ASSERT_MSG( 0, "Crawling Spider colliding with object that has no physics id" );
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: ClearContact
/// Params: [in]idNum, [in]castId, [in]point, [in]pOtherShape, [in]pOtherBody
///
/////////////////////////////////////////////////////
void CrawlingSpider::ClearContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody )
{
	switch( idNum )
	{
		case PHYSICSBASICID_WORLD:
		case PHYSICSBASICID_BULLET:
		{

		}break;

		case PHYSICSBASICID_PLAYER:
		{
			if( m_State != CrawlingSpiderState_Dead )
			{
				if( m_State == CrawlingSpiderState_Attack )
				{
					// tell the system to look for the player again once last anim has finished
					m_ContactWithPlayer = false;
				}
			}
		}break;

		case PHYSICSBASICID_ENEMY:
		case PHYSICSBASICID_FLOATINGHEAD:
		case PHYSICSBASICID_CRAWLINGSPIDER:
		{

		}break;

		default:
			// colliding with something that doesn't have an id
			DBG_ASSERT_MSG( 0, "Crawling Spider removing contact from object that has no physics id" );
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateNavMesh
/// Params: None
///
/////////////////////////////////////////////////////
void CrawlingSpider::UpdateNavMesh( float deltaTime )
{
	float distance;

	if( m_State == CrawlingSpiderState_Walk )
		m_MaxSpeed = MAX_WALK_SPEED;

	float max_distance = m_MaxSpeed * deltaTime;

	if (m_PathActive)
	{
		// Move along the waypoint path
		if (m_NextWaypoint != m_Path.WaypointList().end())
		{
			// determine our desired movement vector
			m_Movement = (*m_NextWaypoint).Position;
			m_Movement -= m_Pos;

			if (m_Movement.X || m_Movement.Z)
			{
				if( m_NewWaypoint )
				{
					math::Vec3 dir = Normalise( m_Movement );
					m_TargetRotation = math::RadToDeg( atan2(dir.X, dir.Z) );
					m_RotationAngle = m_TargetRotation;

					m_NewWaypoint = false;
				}

				if( m_State != CrawlingSpiderState_Walk )
				{
					m_State = CrawlingSpiderState_Walk; 
					SetAnimation( CrawlingSpiderAnim::WALK, 0, true );
				}
			}
		}
		else
		{
			// we have reached the end of the path
			m_PathActive = false;
			m_Movement.X=m_Movement.Y=m_Movement.Z=0.0f;

			if( m_State != CrawlingSpiderState_Idle )
			{
				m_State = CrawlingSpiderState_Idle; 
				SetAnimation( CrawlingSpiderAnim::IDLE, 0, true );
			}
		}
	}
	else
	{
		// apply some friction to our current movement (if any)
		m_Movement.X *= 0.95f;
		m_Movement.Y *= 0.95f;
		m_Movement.Z *= 0.95f;
	}

	// scale back movement by our max speed if needed
	distance = m_Movement.length();
	if (distance > max_distance)
	{
		m_Movement.normalise();
		m_Movement *= max_distance;
	}

	// come to a full stop when we go below a certain threshold
	if (std::fabs(m_Movement.X) < 0.0001f) 
		m_Movement.X = 0.0f;
	if (std::fabs(m_Movement.Y) < 0.0001f) 
		m_Movement.Y = 0.0f;
	if (std::fabs(m_Movement.Z) < 0.0001f) 
		m_Movement.Z = 0.0f;

	if (m_Movement.X || m_Movement.Z)
	{
		// Constrain any remaining Horizontal movement to the parent navigation rink
		if (m_Parent && m_PathActive)
		{
			// compute the next desired location
			math::Vec3 NextPosition = m_Pos + m_Movement;
			NavigationCell* NextCell = 0;

			// test location on the NavigationMesh and resolve collisions
			m_Parent->ResolveMotionOnMesh( m_Pos, m_CurrentCell, NextPosition, &NextCell );

			m_Pos = NextPosition;
			m_CurrentCell = NextCell;
		}
	}
	else if (m_PathActive)
	{
		// If we have no remaining movement, but the path is active,
		// we have arrived at our desired waypoint.
		// Snap to it's position and figure out where to go next
		m_Pos = (*m_NextWaypoint).Position;
		m_Movement.X = m_Movement.Y = m_Movement.Z = 0.0f;
		distance = 0.0f;
		m_NextWaypoint = m_Path.GetFurthestVisibleWayPoint(m_NextWaypoint);

		if (m_NextWaypoint == m_Path.WaypointList().end())
		{
			m_PathActive = false;
			m_Movement.X=m_Movement.Y=m_Movement.Z=0.0f;

			if( m_State != CrawlingSpiderState_Idle )
			{
				m_State = CrawlingSpiderState_Idle; 
				SetAnimation( CrawlingSpiderAnim::IDLE, 0, true );
			}
		}
		else
			m_NewWaypoint = true;
	}

	b2Vec2 bodyPos = m_pBody->GetWorldPoint(zeroVec2);

	if(m_PathActive)
	{
		m_pBody->SetAwake(true);
		
		// this forces a position
		m_pBody->SetTransform( b2Vec2(m_Pos.X, m_Pos.Z), 0.0f );	
	}
	else
	{
		m_PrevPos = m_Pos;

		m_Pos.X = bodyPos.x;
		m_Pos.Z = bodyPos.y;
	}

	// check if the player can be seen
	if( m_CanSeePlayer )
		m_NavMeshEnable = false;
}
		
/////////////////////////////////////////////////////
/// Method: UpdateDirect
/// Params: None
///
/////////////////////////////////////////////////////
void CrawlingSpider::UpdateDirect( float deltaTime )
{
	float distance;

	if( m_State == CrawlingSpiderState_Walk )
		m_MaxSpeed = MAX_WALK_SPEED;

	float max_distance = m_MaxSpeed * deltaTime;

	if( m_CanSeePlayer )
	{
		if( m_State != CrawlingSpiderState_Attack )
		{
			m_Movement = math::Vec3( m_RayContact.x, 0.0f, m_RayContact.y );
			m_Movement -= m_Pos;

			math::Vec3 dir = Normalise( m_Movement );
			m_TargetRotation = math::RadToDeg( atan2(dir.X, dir.Z) );
			m_RotationAngle = m_TargetRotation;
		}
	}
	else
	{
		// apply some friction to our current movement (if any)
		m_Movement.X *= 0.95f;
		m_Movement.Y *= 0.95f;
		m_Movement.Z *= 0.95f;
	}

	// scale back movement by our max speed if needed
	distance = m_Movement.length();
	if (distance > max_distance)
	{
		m_Movement.normalise();
		m_Movement *= max_distance;
	}

	// come to a full stop when we go below a certain threshold
	if (std::fabs(m_Movement.X) < 0.0001f) 
		m_Movement.X = 0.0f;
	if (std::fabs(m_Movement.Y) < 0.0001f) 
		m_Movement.Y = 0.0f;
	if (std::fabs(m_Movement.Z) < 0.0001f) 
		m_Movement.Z = 0.0f;

	if (m_Movement.X || m_Movement.Z)
	{
		// move box2d
		b2Vec2 moveForce( m_Movement.X*1000.0f, m_Movement.Z*1000.0f );
		m_pBody->SetLinearVelocity( moveForce );
	}

	b2Vec2 bodyPos = m_pBody->GetWorldPoint(zeroVec2);

	if( m_CanSeePlayer )
	{
		m_PrevPos = m_Pos;

		m_Pos.X = bodyPos.x;
		m_Pos.Z = bodyPos.y;
	}
	else
	{
		m_PrevPos = m_Pos;

		m_Pos.X = bodyPos.x;
		m_Pos.Z = bodyPos.y;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateIdle
/// Params: None
///
/////////////////////////////////////////////////////
void CrawlingSpider::UpdateIdle( float deltaTime )
{
	b2Vec2 bodyPos = m_pBody->GetWorldPoint(zeroVec2);

	m_PrevPos = m_Pos;

	m_Pos.X = bodyPos.x;
	m_Pos.Z = bodyPos.y;
}

/////////////////////////////////////////////////////
/// Method: ProcessState
/// Params: None
///
/////////////////////////////////////////////////////
void CrawlingSpider::ProcessState()
{
	switch( m_State )
	{
		case CrawlingSpiderState_InActive:
		break;

		case CrawlingSpiderState_Idle:
		{
			if( m_PrevState == CrawlingSpiderState_Attack )
			{
				m_CurrentSearchTime += m_LastDeltaTime;

				// look for the player every so often
				if( (m_CurrentSearchTime >= m_SearchTime) )
				{
					m_PrevState = CrawlingSpiderState_InActive;

					b2Vec2 enemyPos = m_pBody->GetPosition();
					b2Vec2 playerPos = b2Vec2( m_PlayerPosition.X, m_PlayerPosition.Z );

					CrawlingSpiderRayCastCallback rayCallback;
					physics::PhysicsWorldB2D::GetWorld()->RayCast( &rayCallback, enemyPos, playerPos );

					bool clearPathToPlayer = false;

					if( rayCallback.hit )
					{
						if( rayCallback.hitId == PHYSICSBASICID_PLAYER )
							clearPathToPlayer = true;
						else
							clearPathToPlayer = false;

						m_RayContact = rayCallback.hitPoint;
						m_RayNormal = rayCallback.hitNormal;
					}
					else
					{
						clearPathToPlayer = false;
					}

					// check the angle to see if it's within a decent threshold
					if( clearPathToPlayer )
					{
						m_CanSeePlayer = true;

						if( m_State != CrawlingSpiderState_Walk )
						{
							m_State = CrawlingSpiderState_Walk; 
							SetAnimation( CrawlingSpiderAnim::WALK, 0, true );
						}
					}

					// reset timer
					m_SearchTime = math::RandomNumber( SEARCH_TIME_MIN, SEARCH_TIME_MAX );
					m_CurrentSearchTime = 0.0f;
				}
			}
			else
			{
				if( !m_CanSeePlayer )
				{
					m_CurrentSearchTime += m_LastDeltaTime;

					// look for the player every so often
					if( (m_CurrentSearchTime >= m_SearchTime) )
					{
						b2Vec2 enemyPos = m_pBody->GetPosition();
						b2Vec2 playerPos = b2Vec2( m_PlayerPosition.X, m_PlayerPosition.Z );

						CrawlingSpiderRayCastCallback rayCallback;
						physics::PhysicsWorldB2D::GetWorld()->RayCast( &rayCallback, enemyPos, playerPos );

						bool clearPathToPlayer = false;

						if( rayCallback.hit )
						{
							if( rayCallback.hitId == PHYSICSBASICID_PLAYER )
								clearPathToPlayer = true;
							else
								clearPathToPlayer = false;

							m_RayContact = rayCallback.hitPoint;
							m_RayNormal = rayCallback.hitNormal;
						}
						else
						{
							clearPathToPlayer = false;
						}

						// check the angle to see if it's within a decent threshold
						if( clearPathToPlayer )
						{
							math::Vec3 dir( 0.0f, 0.0f, 0.0f );
							dir.X = std::sin( math::DegToRad(m_RotationAngle) );
							dir.Z = std::cos( math::DegToRad(m_RotationAngle) );
							math::Vec3 enemyToPlayer = m_PlayerPosition - m_Pos;
							enemyToPlayer.normalise();

							float angleDiff = math::AngleBetweenVectors( enemyToPlayer, dir );

							const float VIEW_ANGLE = 15.0f;
							const float LEN_RANGE = 10.0f;

							if( (std::fabs( angleDiff ) < math::DegToRad(VIEW_ANGLE)) &&
								(std::fabs(enemyToPlayer.length()) < LEN_RANGE) )
							{
								m_CanSeePlayer = true;

								if( m_State != CrawlingSpiderState_Walk )
								{
									m_State = CrawlingSpiderState_Walk; 
									SetAnimation( CrawlingSpiderAnim::WALK, 0, true );
								}
							}
							else
								m_CanSeePlayer = false;
						}

						// reset timer
						m_SearchTime = math::RandomNumber( SEARCH_TIME_MIN, SEARCH_TIME_MAX );
						m_CurrentSearchTime = 0.0f;
					}
				}
			}
		}break;

		case CrawlingSpiderState_Walk:
		{
			if( !m_NavMeshEnable )
			{
				b2Vec2 enemyPos = m_pBody->GetPosition();
				b2Vec2 playerPos = b2Vec2( m_PlayerPosition.X, m_PlayerPosition.Z );

				CrawlingSpiderRayCastCallback rayCallback;
				physics::PhysicsWorldB2D::GetWorld()->RayCast( &rayCallback, enemyPos, playerPos );

				// check for a state change

				if( rayCallback.hit )
				{
					if( rayCallback.hitId == PHYSICSBASICID_PLAYER )
						m_CanSeePlayer = true;
					else
						m_CanSeePlayer = false;

					m_RayContact = rayCallback.hitPoint;
					m_RayNormal = rayCallback.hitNormal;
				}
				else
				{
					m_CanSeePlayer = false;
				}

				// go to idle or switch to nav mesh
				if( !m_CanSeePlayer )
				{
					m_State = CrawlingSpiderState_Idle; 
					SetAnimation( CrawlingSpiderAnim::IDLE, 0, true );
				}
				else
				{
					if( m_State == CrawlingSpiderState_Walk )
					{
						if( m_LocalPrimaryAnim.nAnimId != CrawlingSpiderAnim::WALK )
						{
							SetAnimation( CrawlingSpiderAnim::WALK, 0, false );
						}
					}

				}
			}
		}break;

		/*case CrawlingSpiderState_Flinch:
		{
			// has flinch anim finished playing
			if( m_LocalPrimaryAnim.nCurrentFrame == m_LocalPrimaryAnim.nTotalFrames  )
			{
				if( m_CanSeePlayer )
				{
					// is player in view, go direct
					if( m_PrevState == EnemyState_Run )
					{
						m_State = EnemyState_Run; 
						SetAnimation( EnemyAnim::RUN, 0, true );
					}
					else
					{
						m_State = EnemyState_Walk; 
						SetAnimation( math::RandomNumber(EnemyAnim::WALK, EnemyAnim::WALK_ALT_RAISED_ARMS), 0, true );
					}
				}
				else
				{
					// generate nav mesh, and chase
					if( m_PrevState == EnemyState_Run )
					{
						m_State = EnemyState_Run; 
						SetAnimation( EnemyAnim::RUN, 0, true );
					}
					else
					{
						m_State = EnemyState_Walk;
						SetAnimation( math::RandomNumber(EnemyAnim::WALK, EnemyAnim::WALK_ALT_RAISED_ARMS), 0, true );
					}
				}

				// there should not be a polled anim
				m_pPolledAnim = 0;
			}
		}break;*/

		case CrawlingSpiderState_Attack:
		{
			m_CanSeePlayer = false;

			// see how far the player got away
			math::Vec3 delta = m_PlayerPosition-m_Pos;

			/*if( (m_LocalPrimaryAnim.nAnimId == CrawlingSpiderAnim::ATTACK_LEFT) ||
				(m_LocalPrimaryAnim.nAnimId == CrawlingSpiderAnim::ATTACK_RIGHT) )
			{
				if( m_LocalPrimaryAnim.nCurrentFrame == PUNCH_FRAME )
				{
					if( std::abs( len ) <= PLAYER_DAMAGE_DISTANCE )
					{
						GameSystems::GetInstance()->GetPlayer().TakeDamage(CRAWLINGSPIDER_DAMAGE);
					}
				}
			}
			else if( (m_LocalPrimaryAnim.nAnimId == CrawlingSpiderAnim::ATTACK_HEADBUTT) )
			{
				if( m_LocalPrimaryAnim.nCurrentFrame == HEADBUTT_FRAME )
				{
					if( std::abs( len ) <= PLAYER_DAMAGE_DISTANCE )
					{
						GameSystems::GetInstance()->GetPlayer().TakeDamage(CRAWLINGSPIDER_DAMAGE);
					}
				}
			}*/

			if( m_ContactWithPlayer )
			{

				/*if( (m_LocalPrimaryAnim.nAnimId < CrawlingSpiderAnim::ATTACK_HEADBUTT) ||
					(m_LocalPrimaryAnim.nAnimId > CrawlingSpiderAnim::ATTACK_RIGHT) )
				{
					SetAnimation( math::RandomNumber(CrawlingSpiderAnim::ATTACK_HEADBUTT, CrawlingSpiderAnim::ATTACK_RIGHT), 0, true );
				}
				else
				{
					if( m_LocalPrimaryAnim.nCurrentFrame >= m_LocalPrimaryAnim.nTotalFrames )
					{
						SetAnimation( math::RandomNumber(CrawlingSpiderAnim::ATTACK_HEADBUTT, CrawlingSpiderAnim::ATTACK_RIGHT), 0, true );
					}
				}*/
			}
			else
			{					
				if( m_LocalPrimaryAnim.nCurrentFrame >= m_LocalPrimaryAnim.nTotalFrames )
				{
					// go for anothe attack
					m_PrevState = CrawlingSpiderState_Attack;
					m_State = CrawlingSpiderState_Idle;
					SetAnimation( CrawlingSpiderAnim::IDLE, 0, false );

					m_SearchTime = 1.0f;
					m_CurrentSearchTime = 0.0f;
				}
			}

		}break;
		

		case CrawlingSpiderState_Dead:
		break;

		default:
			DBG_ASSERT_MSG( 0, "unknown Crawling Spider state" );
			break;
	}
}
