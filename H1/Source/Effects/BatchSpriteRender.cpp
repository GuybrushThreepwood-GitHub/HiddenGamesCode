
/*===================================================================
	File: BatchSpriteRender.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"

#include "Resources/SpriteResources.h"
#include "Resources/TextureResources.h"

#include "Effects/BatchSpriteRender.h"
#include "Player/Player.h"

const float DEFAULT_MAX_SPRITE_DRAW_DISTANCE = 2000.0f;

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
BatchSpriteRender::BatchSpriteRender( )
{
	m_NumBatchesInUse = 0;
	m_pPlayer = 0;

	int i=0;

	for( i=0; i < MAX_BATCHES; ++i )
		m_Batches[i].spriteCount = 0;

	m_AlphaBlend = false;
	m_AlphaBlendValue = 0.0f;

	m_MaxDistance = DEFAULT_MAX_SPRITE_DRAW_DISTANCE;
	m_DrawStyle = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
BatchSpriteRender::~BatchSpriteRender( )
{

}

/////////////////////////////////////////////////////
/// Method: AddSprite
/// Params: None
///
/////////////////////////////////////////////////////
void BatchSpriteRender::AddSprite( int batchId, const collision::AABB& aabb, const math::Vec3& pos, const math::Vec3& dims, int spriteResId )
{
	int i=0;
	SpriteBatch* pBatch = 0;

	for( i=0; i < m_NumBatchesInUse; ++i )
	{
		if( m_Batches[i].batchId == batchId )
		{
			if( m_Batches[i].spriteCount >= MAX_SPRITES_PER_BATCH-1 )
				continue;
			else
			{
				pBatch = &m_Batches[i];
			}
		}
	}

	// batch not found, create a new one
	if( pBatch == 0 )
	{
		m_NumBatchesInUse++;
		if( m_NumBatchesInUse >= MAX_BATCHES-1 )
		{
			m_NumBatchesInUse = MAX_BATCHES-1;
			DBGLOG( "*WARNING* : Out of sprite batches\n" );
			return;
		}

		pBatch = &m_Batches[m_NumBatchesInUse-1];
		pBatch->batchId = batchId;
		pBatch->spriteCount = 0;
		pBatch->batchAABB = aabb;
	}

	//pBatch->batchSphere.vCenterPoint
	pBatch->spriteData[pBatch->spriteCount].pos = pos;
	pBatch->spriteData[pBatch->spriteCount].dims = dims;
	pBatch->spriteData[pBatch->spriteCount].radius = dims.X;

	if( dims.Y > pBatch->spriteData[pBatch->spriteCount].radius )
		pBatch->spriteData[pBatch->spriteCount].radius = dims.Y;
	if( dims.Z > pBatch->spriteData[pBatch->spriteCount].radius )
		pBatch->spriteData[pBatch->spriteCount].radius = dims.Z;

	const res::SpriteResourceStore* sr = res::GetSpriteResource( spriteResId );

	int offset = pBatch->spriteCount*6;
	if( sr != 0 )
	{
		const res::TextureResourceStore* tr = res::GetTextureResource( sr->textureResId );
		if( tr->texId == renderer::INVALID_OBJECT )
		{
			res::LoadTexture( sr->textureResId );
		}
		pBatch->textureId = tr->texId;
		
		pBatch->spriteList[offset].uv	= math::Vec2( sr->uOffset,			sr->vOffset );
		pBatch->spriteList[offset+1].uv	= math::Vec2( sr->uOffset + sr->w,	sr->vOffset );
		pBatch->spriteList[offset+2].uv	= math::Vec2( sr->uOffset,			sr->vOffset + sr->h );
		pBatch->spriteList[offset+3].uv	= math::Vec2( sr->uOffset + sr->w,	sr->vOffset );
		pBatch->spriteList[offset+4].uv	= math::Vec2( sr->uOffset + sr->w,	sr->vOffset + sr->h );
		pBatch->spriteList[offset+5].uv	= math::Vec2( sr->uOffset,			sr->vOffset + sr->h );
	}

	pBatch->spriteList[offset].col		= math::Vec4Lite( 255, 255, 255, 255 );//math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
	pBatch->spriteList[offset+1].col	= math::Vec4Lite( 255, 255, 255, 255 );//math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
	pBatch->spriteList[offset+2].col	= math::Vec4Lite( 255, 255, 255, 255 );//math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
	pBatch->spriteList[offset+3].col	= math::Vec4Lite( 255, 255, 255, 255 );//math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
	pBatch->spriteList[offset+4].col	= math::Vec4Lite( 255, 255, 255, 255 );//math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );
	pBatch->spriteList[offset+5].col	= math::Vec4Lite( 255, 255, 255, 255 );//math::Vec4Lite( s->col.R, s->col.G, s->col.B, s->col.A );

	pBatch->spriteCount++;
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void BatchSpriteRender::Draw( void )
{
	int i=0;

	if( m_pPlayer == 0 )
		return;

	renderer::OpenGL::GetInstance()->DisableVBO();

	if( m_AlphaBlend )
	{
		renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
		renderer::OpenGL::GetInstance()->AlphaMode( true, GL_GREATER, m_AlphaBlendValue );
	}
	else
	{
		renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );
	}

	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	//renderer::OpenGL::GetInstance()->SetCullState( false, GL_BACK );

	renderer::OpenGL::GetInstance()->DisableLighting();

	collision::Sphere playerDraw = m_pPlayer->GetDrawDistanceSphere();

	renderer::OpenGL::GetInstance()->SetColour4ub( 128,128,128,255 );

	for( i=0; i < m_NumBatchesInUse; ++i )
	{
		if( renderer::OpenGL::GetInstance()->AABBInFrustum( m_Batches[i].batchAABB ) &&
			playerDraw.SphereCollidesWithAABB( m_Batches[i].batchAABB ) )
		{
			if( m_Batches[i].spriteCount )
			{
				glClientActiveTexture( GL_TEXTURE0 );
				glEnableClientState( GL_TEXTURE_COORD_ARRAY );

				//renderer::OpenGL::GetInstance()->EnableColourArray();

				if( m_Batches[i].textureId != renderer::INVALID_OBJECT )
					renderer::OpenGL::GetInstance()->BindTexture( m_Batches[i].textureId  );

				glVertexPointer( 3, GL_FLOAT, sizeof(SpriteVert), &m_Batches[i].spriteList[0].v );
				glTexCoordPointer( 2, GL_FLOAT, sizeof(SpriteVert), &m_Batches[i].spriteList[0].uv );
				//glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof(SpriteVert), &m_Batches[i].spriteList[0].col );

				glDrawArrays( GL_TRIANGLES, 0, m_Batches[i].spriteCount*6 );
			}	
		}
	}
	
	//renderer::OpenGL::GetInstance()->SetCullState( true, GL_BACK );
	
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	//renderer::OpenGL::GetInstance()->DisableColourArray();
	glClientActiveTexture( GL_TEXTURE0 );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void BatchSpriteRender::Update( float deltaTime )
{	
	int i=0, j=0, k=0;

	if( m_pPlayer == 0 )
		return;

	// update all sprite positions
	math::Vec3 vEye, vCenter;
	renderer::OpenGL::GetInstance()->GetLookAt( vEye, vCenter );
	GLfloat *mat = renderer::OpenGL::GetInstance()->GetModelViewMatrix();

	// get the right and up vectors
	math::Vec3 right;
	right.X = mat[0];
	right.Y = 0.0f;//mat[4];
	right.Z = mat[8];

	math::Vec3 up;
	up.X = 0.0f;//mat[1];
	up.Y = 1.0f;//mat[5];
	up.Z = 0.0f;//mat[9];

	collision::Sphere playerDraw = m_pPlayer->GetDrawDistanceSphere();

	for( i=0; i < m_NumBatchesInUse; ++i )
	{	
		if( renderer::OpenGL::GetInstance()->AABBInFrustum( m_Batches[i].batchAABB ) &&
			playerDraw.SphereCollidesWithAABB( m_Batches[i].batchAABB ) )
		{
			if( m_Batches[i].spriteCount )
			{
				for( j=0, k=0; j < m_Batches[i].spriteCount*6; j+=6, k++ )
				{
					math::Vec3 pos = m_Batches[i].spriteData[k].pos;
					math::Vec3 dims = m_Batches[i].spriteData[k].dims;
					m_Batches[i].spriteData[k].distance = std::abs((vEye - pos).length());

					if( m_Batches[i].spriteData[k].distance >= m_MaxDistance )
						pos = math::Vec3( -100000.0f, -100000.0f, -100000.0f );

					float hw = dims.Width*0.5f;
					float hh = dims.Height*0.5f;
					float hd = dims.Depth*0.5f;

					pos.Y += hh;

					if( m_DrawStyle == 0 )// full rotation
					{
						m_Batches[i].spriteList[j].v= math::Vec3( pos.X + (-right.X - up.X)*(hw), pos.Y + (-right.Y - up.Y)*(hh), pos.Z + (-right.Z - up.Z)*(hw) );
						m_Batches[i].spriteList[j+1].v = math::Vec3( pos.X + (right.X - up.X)*(hw), pos.Y + (right.Y - up.Y)*(hh), pos.Z + (right.Z - up.Z)*(hw) );
						m_Batches[i].spriteList[j+2].v = math::Vec3( pos.X + (up.X - right.X)*(hw), pos.Y + (up.Y - right.Y)*(hh), pos.Z + (up.Z - right.Z)*(hw) );

						m_Batches[i].spriteList[j+3].v = math::Vec3( pos.X + (right.X - up.X)*(hw), pos.Y + (right.Y - up.Y)*(hh), pos.Z + (right.Z - up.Z)*(hw) );
						m_Batches[i].spriteList[j+4].v = math::Vec3( pos.X + (right.X + up.X)*(hw), pos.Y + (right.Y + up.Y)*(hh), pos.Z + (right.Z + up.Z)*(hw) );
						m_Batches[i].spriteList[j+5].v = math::Vec3( pos.X + (up.X - right.X)*(hw), pos.Y + (up.Y - right.Y)*(hh), pos.Z + (up.Z - right.Z)*(hw) );
					}
					else if( m_DrawStyle == 1 )// Y rotation only
					{
						float sinVal;
						float cosVal;
						math::Vec3 vVector;

						float angle = math::AngleBetweenXZ( vEye, pos );
						math::sinCos( &sinVal, &cosVal, angle ); 

						m_Batches[i].spriteList[j].v	= math::Vec3( pos.X + (-hw), pos.Y + (-hh), pos.Z + (-hd) );
						vVector = pos - m_Batches[i].spriteList[j].v;
						m_Batches[i].spriteList[j].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
						m_Batches[i].spriteList[j].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);

						m_Batches[i].spriteList[j+1].v = math::Vec3( pos.X + (hw), pos.Y + (-hh), pos.Z + (-hd) );
						vVector = pos - m_Batches[i].spriteList[j+1].v;
						m_Batches[i].spriteList[j+1].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
						m_Batches[i].spriteList[j+1].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);

						m_Batches[i].spriteList[j+2].v = math::Vec3( pos.X + (-hw), pos.Y + (hh), pos.Z + (hd) );
						vVector = pos - m_Batches[i].spriteList[j+2].v;
						m_Batches[i].spriteList[j+2].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
						m_Batches[i].spriteList[j+2].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);

						m_Batches[i].spriteList[j+3].v = math::Vec3( pos.X + (hw), pos.Y + (-hh), pos.Z + (-hd) );
						vVector = pos - m_Batches[i].spriteList[j+3].v;
						m_Batches[i].spriteList[j+3].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
						m_Batches[i].spriteList[j+3].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);

						m_Batches[i].spriteList[j+4].v = math::Vec3( pos.X + (hw), pos.Y + (hh), pos.Z + (hd) );
						vVector = pos - m_Batches[i].spriteList[j+4].v;
						m_Batches[i].spriteList[j+4].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
						m_Batches[i].spriteList[j+4].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);

						m_Batches[i].spriteList[j+5].v = math::Vec3( pos.X + (-hw), pos.Y + (hh), pos.Z + (hd) );
						vVector = pos - m_Batches[i].spriteList[j+5].v;
						m_Batches[i].spriteList[j+5].v.X = (float)(pos.X + cosVal*vVector.X + sinVal*vVector.Z);
						m_Batches[i].spriteList[j+5].v.Z = (float)(pos.Z - sinVal*vVector.X + cosVal*vVector.Z);
					}
				}
			}
		}	
	}
}

/////////////////////////////////////////////////////
/// Method: Clear
/// Params: None
///
/////////////////////////////////////////////////////
void BatchSpriteRender::Clear()
{	
	m_NumBatchesInUse = 0;

	int i=0;

	for( i=0; i < MAX_BATCHES; ++i )
		m_Batches[i].spriteCount = 0;
}