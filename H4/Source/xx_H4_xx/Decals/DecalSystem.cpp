
/*===================================================================
	File: DecalSystem.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"

#include "Resources/SpriteResources.h"
#include "Resources/ResourceHelper.h"
#include "Resources/TextureResources.h"

#include "H4.h"
#include "Decals/DecalSystem.h"

namespace
{
	const int BULLET_SPRITE_START = 13;
	const int BULLET_SPRITE_END = 14;

	const int BLOOD_SPRITE_START = 3;
	const int BLOOD_SPRITE_END = 12;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
DecalSystem::DecalSystem()
{
	m_NumActiveBulletHoles = 0;
	m_NumActiveBloodSplats = 0;

	m_NextBulletIndex = 0;
	m_NextBloodSplatIndex = 0;

	m_DecalTex = renderer::INVALID_OBJECT;

	// first blood splat
	if( H4::GetHiResMode() )
	{
		const res::SpriteResourceStore* sr = res::GetSpriteResource( BLOOD_SPRITE_START );
		if( sr && m_DecalTex == renderer::INVALID_OBJECT )
		{
			const res::TextureResourceStore* rs = 0;
			rs = res::GetTextureResource( sr->textureResId );
			DBG_ASSERT( rs != 0 );

			m_DecalTex = rs->texId; 
		}
	}
	else
	{
		renderer::Texture texLoad;
		const res::SpriteResourceStore* sr = res::GetSpriteResource( BLOOD_SPRITE_START );
		if( sr && m_DecalTex == renderer::INVALID_OBJECT )
		{
			m_DecalTex = renderer::TextureLoad( "textures/fx.png", texLoad, renderer::TEXTURE_PNG, 0, false, sr->magFilter, sr->minFilter );
		}
	}
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
DecalSystem::~DecalSystem()
{
	//renderer::RemoveTexture( m_DecalTex );
}

/////////////////////////////////////////////////////
/// Method: AddBulletHole
/// Params: 
///
/////////////////////////////////////////////////////
void DecalSystem::AddBulletHole( const math::Vec3& pos, const math::Vec3& normal )
{
	float angle = 0.0f;
	angle += math::DegToRad( 90.0f*normal.X );
	if( normal.Z < 0.0f )
		angle += math::DegToRad(-180.0f*normal.Z);

	float sinVal;
	float cosVal;
	math::Vec3 vVector;
	math::sinCos( &sinVal, &cosVal, angle ); 

	const res::SpriteResourceStore* sr = res::GetSpriteResource( math::RandomNumber( BULLET_SPRITE_START, BULLET_SPRITE_END ) );

	// first tri
	m_BulletDecals[m_NextBulletIndex].v = math::Vec3( pos.X-BULLET_DECAL_SIZE, pos.Y-BULLET_DECAL_SIZE, pos.Z );
	vVector = m_BulletDecals[m_NextBulletIndex].v - pos;
	m_BulletDecals[m_NextBulletIndex].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
	m_BulletDecals[m_NextBulletIndex].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
	m_BulletDecals[m_NextBulletIndex].uv = math::Vec2( sr->uOffset, sr->vOffset );

	m_BulletDecals[m_NextBulletIndex+1].v = math::Vec3( pos.X+BULLET_DECAL_SIZE, pos.Y-BULLET_DECAL_SIZE, pos.Z );
	vVector = m_BulletDecals[m_NextBulletIndex+1].v - pos;
	m_BulletDecals[m_NextBulletIndex+1].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
	m_BulletDecals[m_NextBulletIndex+1].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
	m_BulletDecals[m_NextBulletIndex+1].uv = math::Vec2( sr->uOffset+sr->w, sr->vOffset );

	m_BulletDecals[m_NextBulletIndex+2].v = math::Vec3( pos.X-BULLET_DECAL_SIZE, pos.Y+BULLET_DECAL_SIZE, pos.Z );
	vVector = m_BulletDecals[m_NextBulletIndex+2].v - pos;
	m_BulletDecals[m_NextBulletIndex+2].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
	m_BulletDecals[m_NextBulletIndex+2].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
	m_BulletDecals[m_NextBulletIndex+2].uv = math::Vec2( sr->uOffset, sr->vOffset+sr->h );

	// second tri
	m_BulletDecals[m_NextBulletIndex+3].v = math::Vec3( pos.X+BULLET_DECAL_SIZE, pos.Y-BULLET_DECAL_SIZE, pos.Z );
	vVector = m_BulletDecals[m_NextBulletIndex+3].v - pos;
	m_BulletDecals[m_NextBulletIndex+3].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
	m_BulletDecals[m_NextBulletIndex+3].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
	m_BulletDecals[m_NextBulletIndex+3].uv = math::Vec2( sr->uOffset+sr->w, sr->vOffset  );

	m_BulletDecals[m_NextBulletIndex+4].v = math::Vec3( pos.X+BULLET_DECAL_SIZE, pos.Y+BULLET_DECAL_SIZE, pos.Z );
	vVector = m_BulletDecals[m_NextBulletIndex+4].v - pos;
	m_BulletDecals[m_NextBulletIndex+4].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
	m_BulletDecals[m_NextBulletIndex+4].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
	m_BulletDecals[m_NextBulletIndex+4].uv = math::Vec2( sr->uOffset+sr->w, sr->vOffset+sr->h );

	m_BulletDecals[m_NextBulletIndex+5].v = math::Vec3( pos.X-BULLET_DECAL_SIZE, pos.Y+BULLET_DECAL_SIZE, pos.Z );
	vVector = m_BulletDecals[m_NextBulletIndex+5].v - pos;
	m_BulletDecals[m_NextBulletIndex+5].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
	m_BulletDecals[m_NextBulletIndex+5].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
	m_BulletDecals[m_NextBulletIndex+5].uv = math::Vec2( sr->uOffset, sr->vOffset+sr->h );

	// next 
	m_NextBulletIndex+=6;
	if( m_NextBulletIndex >= MAX_PLAYER_BULLETS*6 )
		m_NextBulletIndex = 0;
	
	m_NumActiveBulletHoles++;
	if( m_NumActiveBulletHoles >= MAX_PLAYER_BULLETS )
		m_NumActiveBulletHoles = MAX_PLAYER_BULLETS;
}

/////////////////////////////////////////////////////
/// Method: AddBloodSplat
/// Params: 
///
/////////////////////////////////////////////////////
void DecalSystem::AddBloodSplat( bool floorHit, const math::Vec3& pos, const math::Vec3& normal )
{
	const res::SpriteResourceStore* sr = res::GetSpriteResource( math::RandomNumber( BLOOD_SPRITE_START, BLOOD_SPRITE_END ) );

	if( floorHit )
	{
		// first tri
		m_BloodDecals[m_NextBloodSplatIndex].v = math::Vec3( pos.X+BLOOD_DECAL_SIZE, pos.Y, pos.Z-BLOOD_DECAL_SIZE );
		m_BloodDecals[m_NextBloodSplatIndex].uv = math::Vec2( sr->uOffset, sr->vOffset );

		m_BloodDecals[m_NextBloodSplatIndex+1].v = math::Vec3( pos.X-BLOOD_DECAL_SIZE, pos.Y, pos.Z-BLOOD_DECAL_SIZE );
		m_BloodDecals[m_NextBloodSplatIndex+1].uv = math::Vec2( sr->uOffset+sr->w, sr->vOffset );

		m_BloodDecals[m_NextBloodSplatIndex+2].v = math::Vec3( pos.X+BLOOD_DECAL_SIZE, pos.Y, pos.Z+BLOOD_DECAL_SIZE );
		m_BloodDecals[m_NextBloodSplatIndex+2].uv = math::Vec2( sr->uOffset, sr->vOffset+sr->h );

		// second tri
		m_BloodDecals[m_NextBloodSplatIndex+3].v = math::Vec3( pos.X-BLOOD_DECAL_SIZE, pos.Y, pos.Z-BLOOD_DECAL_SIZE );
		m_BloodDecals[m_NextBloodSplatIndex+3].uv = math::Vec2( sr->uOffset+sr->w, sr->vOffset );

		m_BloodDecals[m_NextBloodSplatIndex+4].v = math::Vec3( pos.X-BLOOD_DECAL_SIZE, pos.Y, pos.Z+BLOOD_DECAL_SIZE );
		m_BloodDecals[m_NextBloodSplatIndex+4].uv = math::Vec2( sr->uOffset+sr->w, sr->vOffset+sr->h );

		m_BloodDecals[m_NextBloodSplatIndex+5].v = math::Vec3( pos.X+BLOOD_DECAL_SIZE, pos.Y, pos.Z+BLOOD_DECAL_SIZE );
		m_BloodDecals[m_NextBloodSplatIndex+5].uv = math::Vec2( sr->uOffset, sr->vOffset+sr->h );
	}
	else
	{
		float angle = 0.0f;
		angle += math::DegToRad( 90.0f*normal.X );
		if( normal.Z < 0.0f )
			angle += math::DegToRad(-180.0f*normal.Z);

		float sinVal;
		float cosVal;
		math::Vec3 vVector;
		math::sinCos( &sinVal, &cosVal, angle ); 

		// first tri
		m_BloodDecals[m_NextBloodSplatIndex].v = math::Vec3( pos.X-BLOOD_DECAL_SIZE, pos.Y-BLOOD_DECAL_SIZE, pos.Z );
		vVector = m_BloodDecals[m_NextBloodSplatIndex].v - pos;
		m_BloodDecals[m_NextBloodSplatIndex].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
		m_BloodDecals[m_NextBloodSplatIndex].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
		m_BloodDecals[m_NextBloodSplatIndex].uv = math::Vec2( sr->uOffset, sr->vOffset );

		m_BloodDecals[m_NextBloodSplatIndex+1].v = math::Vec3( pos.X+BLOOD_DECAL_SIZE, pos.Y-BLOOD_DECAL_SIZE, pos.Z );
		vVector = m_BloodDecals[m_NextBloodSplatIndex+1].v - pos;
		m_BloodDecals[m_NextBloodSplatIndex+1].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
		m_BloodDecals[m_NextBloodSplatIndex+1].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
		m_BloodDecals[m_NextBloodSplatIndex+1].uv = math::Vec2( sr->uOffset+sr->w, sr->vOffset );

		m_BloodDecals[m_NextBloodSplatIndex+2].v = math::Vec3( pos.X-BLOOD_DECAL_SIZE, pos.Y+BLOOD_DECAL_SIZE, pos.Z );
		vVector = m_BloodDecals[m_NextBloodSplatIndex+2].v - pos;
		m_BloodDecals[m_NextBloodSplatIndex+2].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
		m_BloodDecals[m_NextBloodSplatIndex+2].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
		m_BloodDecals[m_NextBloodSplatIndex+2].uv = math::Vec2( sr->uOffset, sr->vOffset+sr->h );

		// second tri
		m_BloodDecals[m_NextBloodSplatIndex+3].v = math::Vec3( pos.X+BLOOD_DECAL_SIZE, pos.Y-BLOOD_DECAL_SIZE, pos.Z );
		vVector = m_BloodDecals[m_NextBloodSplatIndex+3].v - pos;
		m_BloodDecals[m_NextBloodSplatIndex+3].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
		m_BloodDecals[m_NextBloodSplatIndex+3].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
		m_BloodDecals[m_NextBloodSplatIndex+3].uv = math::Vec2( sr->uOffset+sr->w, sr->vOffset );

		m_BloodDecals[m_NextBloodSplatIndex+4].v = math::Vec3( pos.X+BLOOD_DECAL_SIZE, pos.Y+BLOOD_DECAL_SIZE, pos.Z );
		vVector = m_BloodDecals[m_NextBloodSplatIndex+4].v - pos;
		m_BloodDecals[m_NextBloodSplatIndex+4].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
		m_BloodDecals[m_NextBloodSplatIndex+4].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
		m_BloodDecals[m_NextBloodSplatIndex+4].uv = math::Vec2( sr->uOffset+sr->w, sr->vOffset+sr->h );

		m_BloodDecals[m_NextBloodSplatIndex+5].v = math::Vec3( pos.X-BLOOD_DECAL_SIZE, pos.Y+BLOOD_DECAL_SIZE, pos.Z );
		vVector = m_BloodDecals[m_NextBloodSplatIndex+5].v - pos;
		m_BloodDecals[m_NextBloodSplatIndex+5].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
		m_BloodDecals[m_NextBloodSplatIndex+5].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
		m_BloodDecals[m_NextBloodSplatIndex+5].uv = math::Vec2( sr->uOffset, sr->vOffset+sr->h );
	}

	// next 
	m_NextBloodSplatIndex+=6;
	if( m_NextBloodSplatIndex >= (MAX_ENEMIES*(MAX_ENEMY_GIBS*MAX_ENEMY_BLOOD_SPLATS))*6 )
		m_NextBloodSplatIndex = 0;

	m_NumActiveBloodSplats++;
	if( m_NumActiveBloodSplats >= (MAX_ENEMIES*(MAX_ENEMY_GIBS*MAX_ENEMY_BLOOD_SPLATS) ) )
		m_NumActiveBloodSplats = (MAX_ENEMIES*(MAX_ENEMY_GIBS*MAX_ENEMY_BLOOD_SPLATS));
}

/////////////////////////////////////////////////////
/// Method: DrawDecals
/// Params: 
///
/////////////////////////////////////////////////////
void DecalSystem::DrawDecals()
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,200 );

	renderer::OpenGL::GetInstance()->EnableTextureArray();
	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );

	renderer::OpenGL::GetInstance()->BindTexture( m_DecalTex );
	
	// DRAW bullets holes
	if( m_NumActiveBulletHoles )
	{
		glVertexPointer( 3, GL_FLOAT, sizeof(DecalVert), &m_BulletDecals[0].v );
		glTexCoordPointer( 2, GL_FLOAT, sizeof(DecalVert), &m_BulletDecals[0].uv );

		glDrawArrays( GL_TRIANGLES, 0, m_NumActiveBulletHoles*6 );
	}

	// draw blood splats
	if( m_NumActiveBloodSplats )
	{
		glVertexPointer( 3, GL_FLOAT, sizeof(DecalVert), &m_BloodDecals[0].v );
		glTexCoordPointer( 2, GL_FLOAT, sizeof(DecalVert), &m_BloodDecals[0].uv );

		glDrawArrays( GL_TRIANGLES, 0, m_NumActiveBloodSplats*6 );
	}

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->DisableTextureArray();

}

/////////////////////////////////////////////////////
/// Method: ClearDecals
/// Params: 
///
/////////////////////////////////////////////////////
void DecalSystem::ClearDecals()
{
	m_NextBloodSplatIndex = 0;
	m_NumActiveBloodSplats = 0;
	
	m_NextBulletIndex = 0;
	m_NumActiveBulletHoles = 0;
}

