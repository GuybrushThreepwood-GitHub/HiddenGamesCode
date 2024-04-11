
/*===================================================================
	File: RoomInstance.cpp
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

#include "Resources/ModelResources.h"
#include "RoomInstance.h"

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
RoomInstance::RoomInstance( const char* modelFile, GLenum magFilter, GLenum minFilter, const math::Vec3& pos, float angle, const math::Vec3& scale )
	: m_pModel(0)
	, m_Pos(pos)
	, m_Angle(angle)
	, m_Scale(scale)
{
	m_pModel = res::LoadModel( modelFile, magFilter, minFilter, true );
	DBG_ASSERT_MSG( (m_pModel != 0), "Could not load or find RoomInstance model" );

	if(m_pModel)
	{
		// setup the bounding area
		m_AABB = m_pModel->modelAABB;
		m_AABB.vBoxMin = math::RotateAroundPoint( m_AABB.vBoxMin, m_AABB.vCenter, math::DegToRad(angle) );
		m_AABB.vBoxMax = math::RotateAroundPoint( m_AABB.vBoxMax, m_AABB.vCenter, math::DegToRad(angle) );

		m_AABB.vCenter = pos;
		m_MinExtend = m_AABB.vBoxMin;
		m_MaxExtend = m_AABB.vBoxMax;
		m_AABB.vBoxMin = m_AABB.vCenter + m_MinExtend;
		m_AABB.vBoxMax = m_AABB.vCenter + m_MaxExtend;
	}
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
RoomInstance::~RoomInstance()
{
	res::RemoveModel(m_pModel);
	m_pModel = 0;
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void RoomInstance::Draw( bool sortedMeshes )
{
	if( !renderer::OpenGL::GetInstance()->AABBInFrustum(m_AABB) )
		return;

	if( m_Scale.X < 0.0f ||
		m_Scale.Y < 0.0f ||
		m_Scale.Z < 0.0f )
	{
		glFrontFace(GL_CW);
	}

	glPushMatrix();
		glTranslatef( m_Pos.X, m_Pos.Y, m_Pos.Z );
		glRotatef( m_Angle, 0.0f, 1.0f, 0.0f );
		glScalef( m_Scale.X, m_Scale.Y, m_Scale.Z );
		if( m_pModel != 0 )
			m_pModel->Draw( sortedMeshes );
	glPopMatrix();

	if( m_Scale.X < 0.0f ||
		m_Scale.Y < 0.0f ||
		m_Scale.Z < 0.0f )
	{
		glFrontFace(GL_CCW);
	}
}
