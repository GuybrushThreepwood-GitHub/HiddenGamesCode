
/*===================================================================
	File: Emitter.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"

#include "GameSystems.h"
#include "Effects/Emitter.h"

using efx::Emitter;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
Emitter::Emitter()
{
	m_SpriteId = -1;

	m_CameraAlign = true;
	m_UsePoints = false;
	m_UseHWPointSprites = false;
	m_SmoothPoints = false;
	m_YRotationOnly = false;

	m_AllParticlesDisabled = false;
	m_ParticleDisableCount = 0;

	m_SystemBoundingBox.Reset();
	m_SystemBoundingSphere.Reset();

	m_EmitEnabled = false;

	m_ViewAlign = true;

	m_TextureID = renderer::INVALID_OBJECT;
	m_EnableDepth = false;
	m_DepthFunc = GL_LESS;

	m_EnableBlending = true;
	m_BlendSrc = GL_SRC_ALPHA;
	m_BlendDest = GL_ONE_MINUS_SRC_ALPHA;
		
	m_EnableAlpha = false;
	m_AlphaCompare = GL_ALWAYS;
	m_AlphaClamp = 0.1f;

	m_Rot = math::Vec3( 0.0f, 0.0f, 0.0f );

	SetSpawnDir1(math::Vec3(-200.0f, -200.0f, -200.0f));
	SetSpawnDir2(math::Vec3( 200.0f, 200.0f,  200.0f));
	SetStartColour1(math::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
	SetStartColour2(math::Vec4(1.0f, 1.0f, 1.0f, 0.0f));
	SetEndColour1(math::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
	SetEndColour2(math::Vec4(1.0f, 1.0f, 1.0f, 0.0f));
	SetPos(math::Vec3(0.0f, -10000.0f, 0.0f));
	SetLifetime(math::CMinMax<float>(0.0f, 0.0f));
	SetEmitRate(math::CMinMax<float>(0.0f, 0.0f));
	SetSize(math::CMinMax<float>(1.0f, 1.0f));

	m_UseWidthHeightDepth = false;
	m_Dims.Width = 1.0f;
	m_Dims.Height = 1.0f;
	m_Dims.Depth = 0.0f;

	m_UseScaling = false;
//	m_EmitRadius = math::Vec3( 1.0f, 1.0f, 1.0f );
  
//	m_MaxParticles = 0;
//	m_Gravity = math::Vec3( 0.0f, 0.0f, 0.0f );

	m_UseRandomColour = true;
	m_RandomColourSwap = false;

	m_pParticles = 0;
	m_ParticleVertList = 0;

	m_PointBufferObject =renderer::INVALID_OBJECT;
	m_TextureBufferObject2D =renderer::INVALID_OBJECT;
	m_ColourBufferObject =renderer::INVALID_OBJECT;

	m_VBOInitialised = false;	

	m_TimeOn = -1.0f;
	m_TimeOff = -1.0f;
	m_CurrentTimer = 0.0f;
	m_TimeToWait = -1.0f;
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
Emitter::~Emitter()
{
	Destroy();
}

/////////////////////////////////////////////////////
/// Method: Create
/// Params: [in]particleCount, [in]pointSize, [in]texID
///
/////////////////////////////////////////////////////
int Emitter::Create( unsigned int particleCount, float pointSize, GLuint texID )
{
	int i = 0, triSpriteOffset = 0;
	
	m_TextureID = texID;

	if( particleCount <= 0 )
		particleCount = 1;

	m_MaxParticles = particleCount;

	m_pParticles = new Particle[m_MaxParticles];
	std::memset( m_pParticles, 0, sizeof(Particle)*m_MaxParticles );

	if( m_UsePoints || m_UseHWPointSprites )
	{
		// single point
		m_ParticleVertList = new ParticleVert[m_MaxParticles];
		for( i=0; i < m_MaxParticles; ++i)
			m_ParticleVertList[i].col.A = 0;
	}
	else
	{
		// triangle strip
		m_ParticleVertList = new ParticleVert[m_MaxParticles*6];

		for( i=0, triSpriteOffset=0; i < m_MaxParticles; ++i, triSpriteOffset+=6 )
		{
			m_ParticleVertList[triSpriteOffset].uv = math::Vec2( 0.0f, 0.0f );
			m_ParticleVertList[triSpriteOffset].col.A = 0;
			m_ParticleVertList[triSpriteOffset+1].uv = math::Vec2( 1.0f, 0.0f );
			m_ParticleVertList[triSpriteOffset+1].col.A = 0;
			m_ParticleVertList[triSpriteOffset+2].uv = math::Vec2( 0.0f, 1.0f );
			m_ParticleVertList[triSpriteOffset+2].col.A = 0;

			m_ParticleVertList[triSpriteOffset+3].uv = math::Vec2( 1.0f, 0.0f );
			m_ParticleVertList[triSpriteOffset+3].col.A = 0;
			m_ParticleVertList[triSpriteOffset+4].uv = math::Vec2( 1.0f, 1.0f );
			m_ParticleVertList[triSpriteOffset+4].col.A = 0;
			m_ParticleVertList[triSpriteOffset+5].uv = math::Vec2( 0.0f, 1.0f );
			m_ParticleVertList[triSpriteOffset+5].col.A = 0;
		}
	}

	//if( m_EmitEnabled )
	//	Reset();

	m_PointSize = pointSize;

	/*Enable();
	for( int i=0; i < 1000; ++i )
	{
		Update(0.5f);
	}
	Disable();*/

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Create
/// Params: [in]emitterBlock
///
/////////////////////////////////////////////////////
int Emitter::Create( EmitterBlock& emitterBlock )
{
	SetPos(math::Vec3(0.0f, -10000.0f, 0.0f));
	
	m_SpriteId = emitterBlock.spriteId;

	m_CameraAlign = emitterBlock.cameraAlign;
	m_UsePoints = emitterBlock.usePoints;
	m_SmoothPoints = emitterBlock.smoothPoints;
	m_UseHWPointSprites = emitterBlock.useHWPointSprites;
	m_YRotationOnly = emitterBlock.yRotationOnly;

	GLuint texId = renderer::INVALID_OBJECT;

	//if( !m_UsePoints )
	//	texId = Resources::LoadTextureResource( emitterBlock.emitterTexture, GL_LINEAR, GL_LINEAR, false );

	m_EnableDepth = emitterBlock.enableDepth;
	m_DepthFunc = emitterBlock.depthFunc;

	m_EnableBlending = emitterBlock.enableBlending;
	m_BlendSrc = emitterBlock.blendSrc;
	m_BlendDest = emitterBlock.blendDest;
		
	m_EnableAlpha = emitterBlock.enableAlpha;
	m_AlphaCompare = emitterBlock.alphaCompare;
	m_AlphaClamp = emitterBlock.alphaClamp;

	SetSpawnDir1( emitterBlock.spawnDir1 );
	SetSpawnDir2( emitterBlock.spawnDir2 );
	SetStartColour1( emitterBlock.startColour1 );
	SetStartColour2( emitterBlock.startColour2 );
	SetEndColour1( emitterBlock.endColour1 );
	SetEndColour2( emitterBlock.endColour2 );
	
	SetLifetime(math::CMinMax<float>(emitterBlock.lifetime.minVal, emitterBlock.lifetime.maxVal));
	SetEmitRate(math::CMinMax<float>(emitterBlock.emitRate.minVal, emitterBlock.emitRate.maxVal));
	SetEmitRadius(emitterBlock.emitRadius);
	SetSize(math::CMinMax<float>(emitterBlock.size.minVal, emitterBlock.size.maxVal));
	SetGravity( emitterBlock.gravity );

	SetUseRandomColour( emitterBlock.useRandomColour );
	SetUseWidthHeightDepth(emitterBlock.useWidthHeightDepth);
	SetDimensions( emitterBlock.dims );
	SetUseScaling(emitterBlock.useScaling);

	Create( emitterBlock.maxParticles, emitterBlock.size.maxVal, texId );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void Emitter::Draw( void )
{
	if( !renderer::OpenGL::GetInstance()->AABBInFrustum(m_SystemBoundingBox) )
		return;

	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();

	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::TGLDepthBlendAlphaState renderState;
	renderer::OpenGL::GetInstance()->SaveDepthBlendAlphaState( &renderState );
	
	if( m_UseHWPointSprites )
		glEnable(GL_POINT_SPRITE);

	if( m_SmoothPoints )
		glEnable(GL_POINT_SMOOTH);

	if( m_UsePoints || m_UseHWPointSprites )
	{
		glPointSize( m_PointSize );
	}

	// setup states
	renderer::OpenGL::GetInstance()->DepthMode( m_EnableDepth, m_DepthFunc );
	renderer::OpenGL::GetInstance()->BlendMode( m_EnableBlending, m_BlendSrc, m_BlendDest );
	renderer::OpenGL::GetInstance()->AlphaMode( m_EnableAlpha, m_AlphaCompare, m_AlphaClamp );

	// if there is no texture use just normal GL points instead of white quads
	if( !m_UsePoints && 
		m_TextureID != renderer::INVALID_OBJECT )
	{
		// bind the texture
		renderer::OpenGL::GetInstance()->BindTexture( m_TextureID );

		if( m_UseHWPointSprites )
			glTexEnvf( GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE );
	}
	else
		renderer::OpenGL::GetInstance()->DisableUnitTexture( 0 );	

	if( !m_CameraAlign )
		renderer::OpenGL::GetInstance()->SetCullState( false, GL_BACK );

	// draw the particles
	if( core::app::GetUseVertexArrays() ||
		!renderer::bExtVertexBufferObjects )
		DrawVA();
	else
		DrawVertexBufferObject();

	if( !m_CameraAlign )
		renderer::OpenGL::GetInstance()->SetCullState( true, GL_BACK );

	if( m_UsePoints || m_UseHWPointSprites )
		glPointSize(1.0f);

	if( m_SmoothPoints )
		glDisable(GL_POINT_SMOOTH);

	if( m_UseHWPointSprites )
	{
		glTexEnvf( GL_POINT_SPRITE, GL_COORD_REPLACE, GL_FALSE );
		glDisable(GL_POINT_SPRITE);
	}

	renderer::OpenGL::GetInstance()->SetDepthBlendAlphaState( &renderState );

	if( lightState )
		renderer::OpenGL::GetInstance()->EnableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Emitter::Update( float deltaTime )
{
	if( m_TimeToWait > 0.0f )
	{
		m_CurrentTimer += deltaTime;
		if( m_CurrentTimer >= m_TimeToWait )
		{
			m_CurrentTimer = 0.0f;
			m_TimeToWait = 0.0f;

			if( m_EmitEnabled )
				Disable();
			else
				Enable();
		}
	}

	// full camera alignment on all axis
	GLfloat *mat = renderer::OpenGL::GetInstance()->GetModelViewMatrix();

	// get the right and up vectors
	m_Right.X = mat[0];
	m_Right.Y = mat[4];
	m_Right.Z = mat[8];

	m_Up.X = mat[1];
	m_Up.Y = mat[5];
	m_Up.Z = mat[9];

	float fRandX = 1.0f;
	float fRandY = 1.0f;
	float fRandZ = 1.0f;

	float fRandR = 1.0f;
	float fRandG = 1.0f;
	float fRandB = 1.0f;
	float fRandA = 1.0f;

	m_SystemBoundingBox.Reset();
	m_SystemBoundingSphere.Reset();

	m_ParticleDisableCount = 0;
	m_AllParticlesDisabled = false;
	int i=0;
	int triSpriteOffset=0;

	for( i=0, triSpriteOffset=0; i < m_MaxParticles; ++i, triSpriteOffset+=6 )
	{
		// check for respawn
		if( m_EmitEnabled &&
			((m_pParticles[i].age >= m_pParticles[i].lifetime) || 
			(m_pParticles[i].colour.A <= 0.0f)) )
		{
			m_AllParticlesDisabled = false;
			m_pParticles[i].colour.A = 0.0f;

			// if emit and life are zero, then stop spawning
			if( m_EmitRate.m_Max <= 0.0f || m_Lifetime.m_Max <= 0.0f )
			{
				m_pParticles[i].age = 0.0f;
				continue;
			}

			m_pParticles[i].lifetime = m_Lifetime.GetRandomNumInRange();

			if( m_UseScaling )
				m_pParticles[i].size = m_Size.m_Min;
			else
				m_pParticles[i].size = m_Size.GetRandomNumInRange();

			if( m_UseWidthHeightDepth )
				m_pParticles[i].dims = m_Dims;
    
			// determine a random vector between dir1 and dir2
			fRandX = math::RandomNumber(m_SpawnDir1.X, m_SpawnDir2.X);
			fRandY = math::RandomNumber(m_SpawnDir1.Y, m_SpawnDir2.Y);
			fRandZ = math::RandomNumber(m_SpawnDir1.Z, m_SpawnDir2.Z);

			m_pParticles[i].dir = math::Vec3(fRandX, fRandY, fRandZ);
			m_pParticles[i].center = m_Pos;

			if( m_UsePoints || m_UseHWPointSprites )
			{
				m_ParticleVertList[i].v = m_Pos;
			}

			// pick a random vector between +/- emitradius
			fRandX = math::RandomNumber(-m_EmitRadius.X, m_EmitRadius.X);
			fRandY = math::RandomNumber(-m_EmitRadius.Y, m_EmitRadius.Y);
			fRandZ = math::RandomNumber(-m_EmitRadius.Z, m_EmitRadius.Z);

			m_pParticles[i].center += math::Vec3(fRandX, fRandY, fRandZ);

			if( m_UsePoints || m_UseHWPointSprites )
			{
				m_ParticleVertList[i].v += math::Vec3(fRandX, fRandY, fRandZ);
			}

			if( m_UseRandomColour )
			{
				fRandR = math::RandomNumber(m_StartColour1.R, m_StartColour2.R);
				fRandG = math::RandomNumber(m_StartColour1.G, m_StartColour2.G);
				fRandB = math::RandomNumber(m_StartColour1.B, m_StartColour2.B);
				fRandA = math::RandomNumber(m_StartColour1.A, m_StartColour2.A);
				m_GeneratedStartColour = math::Vec4(fRandR, fRandG, fRandB, fRandA);

				fRandR = math::RandomNumber(m_EndColour1.R, m_EndColour2.R);
				fRandG = math::RandomNumber(m_EndColour1.G, m_EndColour2.G);
				fRandB = math::RandomNumber(m_EndColour1.B, m_EndColour2.B);
				fRandA = math::RandomNumber(m_EndColour1.A, m_EndColour2.A);
				m_GeneratedEndColour = math::Vec4(fRandR, fRandG, fRandB, fRandA);	
			}
			else
			{
				if( m_RandomColourSwap )
				{
					m_GeneratedStartColour = m_StartColour1;
					m_GeneratedEndColour = m_EndColour1;
				}
				else
				{
					m_GeneratedStartColour = m_StartColour2;
					m_GeneratedEndColour = m_EndColour2;
				}

				m_RandomColourSwap = !m_RandomColourSwap;
			}

			// figure out startcolour and colourstep based on start and end colour.
			m_pParticles[i].colour = m_GeneratedStartColour;
			math::Vec4Lite startCol(static_cast<int>(m_GeneratedStartColour.R*255.0f), static_cast<int>(m_GeneratedStartColour.G*255.0f), static_cast<int>(m_GeneratedStartColour.B*255.0f),static_cast<int>(m_GeneratedStartColour.A*255.0f));

			if( m_UsePoints || m_UseHWPointSprites )
			{
				m_ParticleVertList[i].col = startCol;
			}
			else
			{

				m_ParticleVertList[triSpriteOffset].col = startCol;
				m_ParticleVertList[triSpriteOffset+1].col = startCol;
				m_ParticleVertList[triSpriteOffset+2].col = startCol;
				m_ParticleVertList[triSpriteOffset+3].col = startCol;
				m_ParticleVertList[triSpriteOffset+4].col = startCol;
				m_ParticleVertList[triSpriteOffset+5].col = startCol;
			}

			if( m_pParticles[i].lifetime != 0.0f )
			{
				m_pParticles[i].colourStep = (m_GeneratedEndColour - m_GeneratedStartColour) / m_pParticles[i].lifetime;

				if( m_UseScaling )
					m_pParticles[i].sizeStep = (m_Size.m_Max - m_pParticles[i].size) / m_pParticles[i].lifetime;
			}
			m_pParticles[i].age = 0.0f;
		}

		m_pParticles[i].dir += ( m_Gravity * deltaTime );

		// age the particle
		m_pParticles[i].age += deltaTime;

		// if this particle's age is greater than it's lifetime, it dies.
		if( !m_EmitEnabled )
		{
			if(m_GeneratedEndColour.A == m_GeneratedStartColour.A  )
			{
				m_pParticles[i].age = 0.0f;
				m_pParticles[i].colour.A = 0.0f;

				for( i=0, triSpriteOffset=0; i < m_MaxParticles; ++i, triSpriteOffset+=6 )
				{
					if( m_UsePoints || m_UseHWPointSprites )
					{
						m_ParticleVertList[i].col = math::Vec4Lite( 0, 0, 0, 0 );
					}
					else
					{
						m_ParticleVertList[triSpriteOffset].col = math::Vec4Lite( 0, 0, 0, 0 );
						m_ParticleVertList[triSpriteOffset+1].col = math::Vec4Lite( 0, 0, 0, 0 );
						m_ParticleVertList[triSpriteOffset+2].col = math::Vec4Lite( 0, 0, 0, 0 );

						m_ParticleVertList[triSpriteOffset+3].col = math::Vec4Lite( 0, 0, 0, 0 );
						m_ParticleVertList[triSpriteOffset+4].col = math::Vec4Lite( 0, 0, 0, 0 );
						m_ParticleVertList[triSpriteOffset+5].col = math::Vec4Lite( 0, 0, 0, 0 );
					}
				}
			}
			else if(m_pParticles[i].age >= m_pParticles[i].lifetime )
			{
				m_pParticles[i].age = 0.0f;
				m_ParticleDisableCount++;
				//m_pParticles[i].colour.A = 0.0f;
			}
			else
			{
				MoveParticle( i, triSpriteOffset, deltaTime );
			}

		}
		else if (m_pParticles[i].age >= m_pParticles[i].lifetime || 
			m_pParticles[i].colour.A <= 0.0f ) 
		{
			// dead!
			//m_ParticleDisableCount++;
		}
		else
		{
			m_AllParticlesDisabled = false;
			MoveParticle( i, triSpriteOffset, deltaTime );
		}

		m_BoxMinSize = math::Vec3( m_pParticles[i].center.X-m_pParticles[i].size, 
										m_pParticles[i].center.Y-m_pParticles[i].size, 
										m_pParticles[i].center.Z-m_pParticles[i].size );

		m_BoxMaxSize = math::Vec3( m_pParticles[i].center.X+m_pParticles[i].size, 
										m_pParticles[i].center.Y+m_pParticles[i].size, 
										m_pParticles[i].center.Z+m_pParticles[i].size );
	
		m_SystemBoundingBox.ReCalculate( m_BoxMinSize,  m_BoxMaxSize );
		m_SystemBoundingSphere.ReCalculate( m_BoxMaxSize - m_Pos );
	}
		
	if( m_ParticleDisableCount >= m_MaxParticles )
		m_AllParticlesDisabled = true;
}

/////////////////////////////////////////////////////
/// Method: Stop
/// Params: None
///
/////////////////////////////////////////////////////
void Emitter::Stop( void )
{
	for( int i=0, triSpriteOffset=0; i < m_MaxParticles; ++i, triSpriteOffset+=6 )
	{
		m_pParticles[i].age = -1.0f;
		m_pParticles[i].lifetime = 0.0f;
		m_pParticles[i].colour.A = 0.0f;
	}
}

/////////////////////////////////////////////////////
/// Method: Reset
/// Params: None
///
/////////////////////////////////////////////////////
int Emitter::Reset( void )
{
	float fRandX = 1.0f;
	float fRandY = 1.0f;
	float fRandZ = 1.0f;

	float fRandR = 1.0f;
	float fRandG = 1.0f;
	float fRandB = 1.0f;
	float fRandA = 1.0f;

	int i=0;
	int triSpriteOffset=0;

	for( i=0, triSpriteOffset=0; i < m_MaxParticles; ++i, triSpriteOffset+=6 )
	{
		m_pParticles[i].age = 0.0f;
		m_pParticles[i].lifetime = m_Lifetime.GetRandomNumInRange();

		if( m_UseScaling )
			m_pParticles[i].size = m_Size.m_Min;
		else
			m_pParticles[i].size = m_Size.GetRandomNumInRange();

		if( m_UseWidthHeightDepth )
			m_pParticles[i].dims = m_Dims;

		// determine a random vector between dir1 and dir2
		fRandX = math::RandomNumber(m_SpawnDir1.X, m_SpawnDir2.X);
		fRandY = math::RandomNumber(m_SpawnDir1.Y, m_SpawnDir2.Y);
		fRandZ = math::RandomNumber(m_SpawnDir1.Z, m_SpawnDir2.Z);

		m_pParticles[i].dir = math::Vec3(fRandX, fRandY, fRandZ);
		m_pParticles[i].center = m_Pos;

		if( m_UsePoints || m_UseHWPointSprites )
		{
			//m_pParticlePositions[i] = m_Pos;
			m_ParticleVertList[i].v = m_Pos;
		}

		// pick a random vector between +/- emitradius
		fRandX = math::RandomNumber(-m_EmitRadius.X, m_EmitRadius.X);
		fRandY = math::RandomNumber(-m_EmitRadius.Y, m_EmitRadius.Y);
		fRandZ = math::RandomNumber(-m_EmitRadius.Z, m_EmitRadius.Z);

		m_pParticles[i].center += math::Vec3(fRandX, fRandY, fRandZ);

		if( m_UsePoints || m_UseHWPointSprites )
		{
			m_ParticleVertList[i].v += math::Vec3(fRandX, fRandY, fRandZ);
		}
    
			
		if( m_UseRandomColour )
		{
			fRandR = math::RandomNumber(m_StartColour1.R, m_StartColour2.R);
			fRandG = math::RandomNumber(m_StartColour1.G, m_StartColour2.G);
			fRandB = math::RandomNumber(m_StartColour1.B, m_StartColour2.B);
			fRandA = math::RandomNumber(m_StartColour1.A, m_StartColour2.A);
			m_GeneratedStartColour = math::Vec4(fRandR, fRandG, fRandB, fRandA);

			fRandR = math::RandomNumber(m_EndColour1.R, m_EndColour2.R);
			fRandG = math::RandomNumber(m_EndColour1.G, m_EndColour2.G);
			fRandB = math::RandomNumber(m_EndColour1.B, m_EndColour2.B);
			fRandA = math::RandomNumber(m_EndColour1.A, m_EndColour2.A);
			m_GeneratedEndColour = math::Vec4(fRandR, fRandG, fRandB, fRandA);
				
		}
		else
		{
			if( m_RandomColourSwap )
			{
				m_GeneratedStartColour = m_StartColour1;
				m_GeneratedEndColour = m_EndColour1;
			}
			else
			{
				m_GeneratedStartColour = m_StartColour2;
				m_GeneratedEndColour = m_EndColour2;
			}

			m_RandomColourSwap = !m_RandomColourSwap;
		}

		// figure out startcolour and colourstep based on start and end colour.
		m_pParticles[i].colour = m_GeneratedStartColour;
		
		math::Vec4Lite startCol(static_cast<int>(m_GeneratedStartColour.R*255.0f), static_cast<int>(m_GeneratedStartColour.G*255.0f), static_cast<int>(m_GeneratedStartColour.B*255.0f),static_cast<int>(m_GeneratedStartColour.A*255.0f));

		if( m_UsePoints || m_UseHWPointSprites )
		{
			m_ParticleVertList[i].col = startCol;
		}
		else
		{
			m_ParticleVertList[triSpriteOffset].col = startCol;
			m_ParticleVertList[triSpriteOffset+1].col = startCol;
			m_ParticleVertList[triSpriteOffset+2].col = startCol;

			m_ParticleVertList[triSpriteOffset+3].col = startCol;
			m_ParticleVertList[triSpriteOffset+4].col = startCol;
			m_ParticleVertList[triSpriteOffset+5].col = startCol;
		}

		if( m_pParticles[i].lifetime != 0.0f )
		{
			m_pParticles[i].colourStep = (m_GeneratedEndColour-m_GeneratedStartColour) / m_pParticles[i].lifetime;

			if( m_UseScaling )
				m_pParticles[i].sizeStep = (m_Size.m_Max - m_pParticles[i].size) / m_pParticles[i].lifetime;
		}

		if( m_CameraAlign == false && 
			m_UseWidthHeightDepth )
		{
			m_BoxMinSize = math::Vec3( m_pParticles[i].center.X-m_pParticles[i].dims.Width, 
											m_pParticles[i].center.Y-m_pParticles[i].dims.Height, 
											m_pParticles[i].center.Z-m_pParticles[i].dims.Width );

			m_BoxMaxSize = math::Vec3( m_pParticles[i].center.X+m_pParticles[i].dims.Width, 
											m_pParticles[i].center.Y+m_pParticles[i].dims.Height, 
											m_pParticles[i].center.Z+m_pParticles[i].dims.Width );
		}
		else
		{
			m_BoxMinSize = math::Vec3( m_pParticles[i].center.X-m_pParticles[i].size, 
											m_pParticles[i].center.Y-m_pParticles[i].size, 
											m_pParticles[i].center.Z-m_pParticles[i].size );

			m_BoxMaxSize = math::Vec3( m_pParticles[i].center.X+m_pParticles[i].size, 
											m_pParticles[i].center.Y+m_pParticles[i].size, 
											m_pParticles[i].center.Z+m_pParticles[i].size );
		}

		m_SystemBoundingBox.ReCalculate( m_BoxMinSize,  m_BoxMaxSize );
		m_SystemBoundingSphere.ReCalculate( m_BoxMaxSize - m_Pos );
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Destroy
/// Params: None
///
/////////////////////////////////////////////////////
int Emitter::Destroy( void )
{
	if( m_pParticles != 0 )
	{
		delete[] m_pParticles;
		m_pParticles = 0;
	}
	if( m_ParticleVertList != 0 )
	{
		delete[] m_ParticleVertList;
		m_ParticleVertList = 0;
	}

	/*if( m_TextureID != renderer::INVALID_OBJECT )
	{
		renderer::RemoveTexture( m_TextureID );
		
	}*/m_TextureID = renderer::INVALID_OBJECT;

	m_VBOInitialised = false;
	
	if( m_PointBufferObject !=renderer::INVALID_OBJECT )
	{
		glDeleteBuffers( 1, &m_PointBufferObject );
		m_PointBufferObject =renderer::INVALID_OBJECT;
	}
	if( m_TextureBufferObject2D !=renderer::INVALID_OBJECT )
	{
		glDeleteBuffers( 1, &m_TextureBufferObject2D );
		m_TextureBufferObject2D =renderer::INVALID_OBJECT;
	}
	if( m_ColourBufferObject !=renderer::INVALID_OBJECT )
	{
		glDeleteBuffers( 1, &m_ColourBufferObject );
		m_ColourBufferObject =renderer::INVALID_OBJECT;
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Method: SetTextureUVOffset
/// Params: [in]nX, [in]nY, [in]nWidth, [in]nHeight, [in]nTextureWidth, [in]nTextureHeight
///
/////////////////////////////////////////////////////
void Emitter::SetTextureUVOffset( int nX, int nY, int nWidth, int nHeight, int nTextureWidth, int nTextureHeight )
{
	int i = 0, triSpriteOffset = 0;

	// bottom left
	m_UVCoords[0].U = renderer::GetUVCoord( nX, nTextureWidth );
	m_UVCoords[0].V = renderer::GetUVCoord( nY, nTextureHeight );

	// bottom right
	m_UVCoords[1].U = renderer::GetUVCoord( nX+nWidth, nTextureWidth );
	m_UVCoords[1].V = renderer::GetUVCoord( nY, nTextureHeight );

	// top left
	m_UVCoords[2].U = renderer::GetUVCoord( nX, nTextureWidth );
	m_UVCoords[2].V = renderer::GetUVCoord( nY+nHeight, nTextureHeight );

	// top right
	m_UVCoords[3].U = renderer::GetUVCoord( nX+nWidth, nTextureWidth );
	m_UVCoords[3].V = renderer::GetUVCoord( nY+nHeight, nTextureHeight );

	if( !m_UsePoints && !m_UseHWPointSprites )
	{
		for( i=0, triSpriteOffset=0; i < m_MaxParticles; ++i, triSpriteOffset+=6 )
		{
			m_ParticleVertList[triSpriteOffset].uv = m_UVCoords[0];
			m_ParticleVertList[triSpriteOffset+1].uv = m_UVCoords[1];
			m_ParticleVertList[triSpriteOffset+2].uv = m_UVCoords[2];

			m_ParticleVertList[triSpriteOffset+3].uv = m_UVCoords[1];
			m_ParticleVertList[triSpriteOffset+4].uv = m_UVCoords[3];
			m_ParticleVertList[triSpriteOffset+5].uv = m_UVCoords[2];
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SetTextureUVOffset
/// Params: [in]uOffset, [in]uOffset, [in]w, [in]h
///
/////////////////////////////////////////////////////
void Emitter::SetTextureUVOffset( float uOffset, float vOffset, float w, float h )
{
	int i = 0, triSpriteOffset = 0;

	// bottom left
	m_UVCoords[0].U = uOffset;
	m_UVCoords[0].V = vOffset;

	// bottom right
	m_UVCoords[1].U = uOffset+w;
	m_UVCoords[1].V = vOffset;

	// top left
	m_UVCoords[2].U = uOffset;
	m_UVCoords[2].V = vOffset+h;

	// top right
	m_UVCoords[3].U = uOffset+w;
	m_UVCoords[3].V = vOffset+h;

	if( !m_UsePoints && !m_UseHWPointSprites )
	{
		for( i=0, triSpriteOffset=0; i < m_MaxParticles; ++i, triSpriteOffset+=6 )
		{
			m_ParticleVertList[triSpriteOffset].uv = m_UVCoords[0];
			m_ParticleVertList[triSpriteOffset+1].uv = m_UVCoords[1];
			m_ParticleVertList[triSpriteOffset+2].uv = m_UVCoords[2];

			m_ParticleVertList[triSpriteOffset+3].uv = m_UVCoords[1];
			m_ParticleVertList[triSpriteOffset+4].uv = m_UVCoords[3];
			m_ParticleVertList[triSpriteOffset+5].uv = m_UVCoords[2];
		}

			// texture coords VBO
		if( m_TextureBufferObject2D !=renderer::INVALID_OBJECT )
		{						
			glBindBuffer( GL_ARRAY_BUFFER, m_TextureBufferObject2D );		
			
			glBufferData( GL_ARRAY_BUFFER, (m_MaxParticles*sizeof(ParticleVert))*6, &m_ParticleVertList[0].uv[0], GL_STATIC_DRAW );
			glTexCoordPointer( 2, GL_FLOAT, sizeof( ParticleVert ), &m_ParticleVertList[0].uv );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: MoveParticle
/// Params: None
///
/////////////////////////////////////////////////////
void Emitter::MoveParticle( int i, int triSpriteOffset, float deltaTime )
{
	// move particle
	m_pParticles[i].center += m_pParticles[i].dir * deltaTime;
	m_pParticles[i].colour += m_pParticles[i].colourStep * deltaTime;

	if( m_UseScaling )
	{
		m_pParticles[i].size += m_pParticles[i].sizeStep * deltaTime;

		if( m_pParticles[i].size > m_Size.m_Max )
			m_pParticles[i].size = m_Size.m_Max;
	}

	if( m_UsePoints || m_UseHWPointSprites )
	{
		m_ParticleVertList[i].v += m_pParticles[i].dir * deltaTime;

		UpdateAndClampColour( i, i, deltaTime );
	}
	else
	{
		if( m_CameraAlign )
		{
			float particleSizeW;
			float particleSizeH;
			float particleSizeD;

			math::Vec3 BottomLeft;
			math::Vec3 BottomRight;
			math::Vec3 TopLeft;
			math::Vec3 TopRight;

			if( m_UseWidthHeightDepth )
			{
				particleSizeW = m_Dims.Width*0.5f;
				particleSizeH = m_Dims.Height*0.5f;
				particleSizeD = m_Dims.Depth*0.5f;

				if( m_UseScaling )
				{
					particleSizeW *= m_pParticles[i].size;
					particleSizeH *= m_pParticles[i].size;
					particleSizeD *= m_pParticles[i].size;
				}
			}
			else
			{
				particleSizeW = m_pParticles[i].size*0.5f;
				particleSizeH = m_pParticles[i].size*0.5f;
				particleSizeD = m_pParticles[i].size*0.5f;
			}

			if( m_YRotationOnly )
			{
				BottomLeft = math::Vec3( (m_pParticles[i].center.X) - particleSizeW, (m_pParticles[i].center.Y) - particleSizeH, m_pParticles[i].center.Z );
				BottomRight = math::Vec3( (m_pParticles[i].center.X) + particleSizeW, (m_pParticles[i].center.Y) - particleSizeH, m_pParticles[i].center.Z );
				TopLeft = math::Vec3( (m_pParticles[i].center.X) - particleSizeW, (m_pParticles[i].center.Y) + particleSizeH, m_pParticles[i].center.Z );
				TopRight = math::Vec3( (m_pParticles[i].center.X) + particleSizeW, (m_pParticles[i].center.Y) + particleSizeH, m_pParticles[i].center.Z );

				BottomLeft = math::RotateAroundPoint( BottomLeft, m_pParticles[i].center, math::DegToRad(GameSystems::GetInstance()->GetGameCamera()->GetAngle()) );
				BottomRight = math::RotateAroundPoint( BottomRight, m_pParticles[i].center, math::DegToRad(GameSystems::GetInstance()->GetGameCamera()->GetAngle()) );
				TopLeft = math::RotateAroundPoint( TopLeft, m_pParticles[i].center, math::DegToRad(GameSystems::GetInstance()->GetGameCamera()->GetAngle()) );
				TopRight = math::RotateAroundPoint( TopRight, m_pParticles[i].center, math::DegToRad(GameSystems::GetInstance()->GetGameCamera()->GetAngle()) );
			}
			else
			{
				BottomLeft = math::Vec3( (m_pParticles[i].center.X) + (-m_Right.X - m_Up.X)*particleSizeW, (m_pParticles[i].center.Y) + (-m_Right.Y - m_Up.Y)*particleSizeH, m_pParticles[i].center.Z + (-m_Right.Z - m_Up.Z)*particleSizeD );
				BottomRight= math::Vec3( (m_pParticles[i].center.X) + (m_Right.X - m_Up.X)*particleSizeW, (m_pParticles[i].center.Y) + (m_Right.Y - m_Up.Y)*particleSizeH, m_pParticles[i].center.Z + (m_Right.Z - m_Up.Z)*particleSizeD );
				TopLeft = math::Vec3( (m_pParticles[i].center.X) + (m_Up.X - m_Right.X)*particleSizeW, (m_pParticles[i].center.Y) + (m_Up.Y - m_Right.Y)*particleSizeH, m_pParticles[i].center.Z + (m_Up.Z - m_Right.Z)*particleSizeD );
				TopRight = math::Vec3( (m_pParticles[i].center.X) + (m_Right.X + m_Up.X)*particleSizeW, (m_pParticles[i].center.Y) + (m_Right.Y + m_Up.Y)*particleSizeH, m_pParticles[i].center.Z + (m_Right.Z + m_Up.Z)*particleSizeD );
			}

			m_ParticleVertList[triSpriteOffset].v = BottomLeft;
			m_ParticleVertList[triSpriteOffset+1].v = BottomRight;
			m_ParticleVertList[triSpriteOffset+2].v = TopLeft;

			m_ParticleVertList[triSpriteOffset+3].v = BottomRight;
			m_ParticleVertList[triSpriteOffset+4].v = TopRight;
			m_ParticleVertList[triSpriteOffset+5].v = TopLeft;
		}
		else
		{
			float particleSizeW;
			float particleSizeH;
			float particleSizeD;

			float sinVal;
			float cosVal;
			math::Vec3 vVector;
			math::sinCos( &sinVal, &cosVal, math::DegToRad(m_Rot.Y) ); 

			if( m_UseWidthHeightDepth )
			{
				particleSizeW = m_Dims.Width*0.5f;
				particleSizeH = m_Dims.Height*0.5f;
				particleSizeD = m_Dims.Depth*0.5f;

				if( m_UseScaling )
				{
					particleSizeW *= m_pParticles[i].size;
					particleSizeH *= m_pParticles[i].size;
					particleSizeD *= m_pParticles[i].size;
				}
			}
			else
			{
				particleSizeW = m_pParticles[i].size*0.5f;
				particleSizeH = m_pParticles[i].size*0.5f;
				particleSizeD = 0.0f;
			}

			m_ParticleVertList[triSpriteOffset].v	= math::Vec3( m_pParticles[i].center.X - particleSizeW, m_pParticles[i].center.Y - particleSizeH, m_pParticles[i].center.Z - particleSizeD );
			vVector = m_ParticleVertList[triSpriteOffset].v - m_pParticles[i].center;
			m_ParticleVertList[triSpriteOffset].v.X = (float)(m_pParticles[i].center.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_ParticleVertList[triSpriteOffset].v.Z = (float)(m_pParticles[i].center.Z - sinVal*vVector.X + cosVal*vVector.Z);

			m_ParticleVertList[triSpriteOffset+1].v = math::Vec3( m_pParticles[i].center.X + particleSizeW, m_pParticles[i].center.Y - particleSizeH, m_pParticles[i].center.Z - particleSizeD );
			vVector = m_ParticleVertList[triSpriteOffset+1].v - m_pParticles[i].center;
			m_ParticleVertList[triSpriteOffset+1].v.X = (float)(m_pParticles[i].center.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_ParticleVertList[triSpriteOffset+1].v.Z = (float)(m_pParticles[i].center.Z - sinVal*vVector.X + cosVal*vVector.Z);

			m_ParticleVertList[triSpriteOffset+2].v = math::Vec3( m_pParticles[i].center.X - particleSizeW, m_pParticles[i].center.Y + particleSizeH, m_pParticles[i].center.Z + particleSizeD );
			vVector = m_ParticleVertList[triSpriteOffset+2].v - m_pParticles[i].center;
			m_ParticleVertList[triSpriteOffset+2].v.X = (float)(m_pParticles[i].center.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_ParticleVertList[triSpriteOffset+2].v.Z = (float)(m_pParticles[i].center.Z - sinVal*vVector.X + cosVal*vVector.Z);

			m_ParticleVertList[triSpriteOffset+3].v = math::Vec3( m_pParticles[i].center.X + particleSizeW, m_pParticles[i].center.Y - particleSizeH, m_pParticles[i].center.Z - particleSizeD );
			vVector = m_ParticleVertList[triSpriteOffset+3].v - m_pParticles[i].center;
			m_ParticleVertList[triSpriteOffset+3].v.X = (float)(m_pParticles[i].center.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_ParticleVertList[triSpriteOffset+3].v.Z = (float)(m_pParticles[i].center.Z - sinVal*vVector.X + cosVal*vVector.Z);

			m_ParticleVertList[triSpriteOffset+4].v = math::Vec3( m_pParticles[i].center.X + particleSizeW, m_pParticles[i].center.Y + particleSizeH, m_pParticles[i].center.Z + particleSizeD );
			vVector = m_ParticleVertList[triSpriteOffset+4].v - m_pParticles[i].center;
			m_ParticleVertList[triSpriteOffset+4].v.X = (float)(m_pParticles[i].center.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_ParticleVertList[triSpriteOffset+4].v.Z = (float)(m_pParticles[i].center.Z - sinVal*vVector.X + cosVal*vVector.Z);

			m_ParticleVertList[triSpriteOffset+5].v = math::Vec3( m_pParticles[i].center.X - particleSizeW, m_pParticles[i].center.Y + particleSizeH, m_pParticles[i].center.Z + particleSizeD );
			vVector = m_ParticleVertList[triSpriteOffset+5].v - m_pParticles[i].center;
			m_ParticleVertList[triSpriteOffset+5].v.X = (float)(m_pParticles[i].center.X + cosVal*vVector.X + sinVal*vVector.Z);
			m_ParticleVertList[triSpriteOffset+5].v.Z = (float)(m_pParticles[i].center.Z - sinVal*vVector.X + cosVal*vVector.Z);

		}
		UpdateAndClampColour( triSpriteOffset, i, deltaTime );
		UpdateAndClampColour( triSpriteOffset+1, i, deltaTime );
		UpdateAndClampColour( triSpriteOffset+2, i, deltaTime );
		UpdateAndClampColour( triSpriteOffset+3, i, deltaTime );
		UpdateAndClampColour( triSpriteOffset+4, i, deltaTime );
		UpdateAndClampColour( triSpriteOffset+5, i, deltaTime );
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateAndClampColour
/// Params: [in]vertIndex, [in]particleIndex, [in]deltaTime
///
/////////////////////////////////////////////////////
void Emitter::UpdateAndClampColour( int vertIndex, int particleIndex, float deltaTime )
{
	int r, g, b, a;

	r = m_ParticleVertList[vertIndex].col.R;
	g = m_ParticleVertList[vertIndex].col.G;
	b = m_ParticleVertList[vertIndex].col.B;
	a = m_ParticleVertList[vertIndex].col.A;

	r += static_cast<int>((m_pParticles[particleIndex].colourStep.R * deltaTime)*255.0f);
	g += static_cast<int>((m_pParticles[particleIndex].colourStep.G * deltaTime)*255.0f);
	b += static_cast<int>((m_pParticles[particleIndex].colourStep.B * deltaTime)*255.0f);
	a += static_cast<int>((m_pParticles[particleIndex].colourStep.A * deltaTime)*255.0f);

	math::Clamp( &r, 0, 255 );
	math::Clamp( &g, 0, 255 );
	math::Clamp( &b, 0, 255 );
	math::Clamp( &a, 0, 255 );

	m_ParticleVertList[vertIndex].col.R = r;
	m_ParticleVertList[vertIndex].col.G = g;
	m_ParticleVertList[vertIndex].col.B = b;
	m_ParticleVertList[vertIndex].col.A = a;
}

/////////////////////////////////////////////////////
/// Method: SetupVertexBufferObject
/// Params: None
///
/////////////////////////////////////////////////////
void Emitter::SetupVertexBufferObject( void )
{
	if( core::app::GetUseVertexArrays() ||
		!renderer::bExtVertexBufferObjects)
		return;

	if( !m_VBOInitialised && m_MaxParticles )
	{
		if( m_UsePoints || m_UseHWPointSprites )
		{
			// point VBO
			glGenBuffers( 1, &m_PointBufferObject );						
			glBindBuffer( GL_ARRAY_BUFFER, m_PointBufferObject );

			glBufferData( GL_ARRAY_BUFFER, m_MaxParticles*sizeof(ParticleVert), &m_ParticleVertList[0].v[0], GL_DYNAMIC_DRAW );
			glVertexPointer( 3, GL_FLOAT, sizeof(ParticleVert), &m_ParticleVertList[0].v );

			// texture coords VBO
			//	//  default diffuse
			//	glGenBuffers( 1, &m_TextureBufferObject2D );						
			//	glBindBuffer( GL_ARRAY_BUFFER, m_TextureBufferObject2D );		
			//
			//	glBufferData( GL_ARRAY_BUFFER, m_MaxParticles*sizeof(math::Vec2), m_pParticleTextureCoords, GL_DYNAMIC_DRAW );
			//	glTexCoordPointer( 2, GL_FLOAT, sizeof( math::Vec2 ), m_pParticleTextureCoords );
			
			// colours VBO
			glGenBuffers( 1, &m_ColourBufferObject );						
			glBindBuffer( GL_ARRAY_BUFFER, m_ColourBufferObject );	

			glBufferData( GL_ARRAY_BUFFER, m_MaxParticles*sizeof(ParticleVert), &m_ParticleVertList[0].col.R, GL_DYNAMIC_DRAW );
			glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( ParticleVert ), &m_ParticleVertList[0].col );
		}
		else
		{
			// vert VBO
			glGenBuffers( 1, &m_PointBufferObject );						
			glBindBuffer( GL_ARRAY_BUFFER, m_PointBufferObject );

			glBufferData( GL_ARRAY_BUFFER, (m_MaxParticles*sizeof(ParticleVert))*6, &m_ParticleVertList[0].v[0], GL_DYNAMIC_DRAW );
			glVertexPointer( 3, GL_FLOAT, sizeof(ParticleVert), &m_ParticleVertList[0].v );

			// texture coords VBO
			glGenBuffers( 1, &m_TextureBufferObject2D );						
			glBindBuffer( GL_ARRAY_BUFFER, m_TextureBufferObject2D );		
			
			glBufferData( GL_ARRAY_BUFFER, (m_MaxParticles*sizeof(ParticleVert))*6, &m_ParticleVertList[0].uv[0], GL_STATIC_DRAW );
			glTexCoordPointer( 2, GL_FLOAT, sizeof( ParticleVert ), &m_ParticleVertList[0].uv );

			// colours VBO
			glGenBuffers( 1, &m_ColourBufferObject );						
			glBindBuffer( GL_ARRAY_BUFFER, m_ColourBufferObject );		

			glBufferData( GL_ARRAY_BUFFER, (m_MaxParticles*sizeof(ParticleVert))*6, &m_ParticleVertList[0].col.R, GL_DYNAMIC_DRAW );
			glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( ParticleVert ), &m_ParticleVertList[0].col );
		}

		m_VBOInitialised = true;
	}
	
	renderer::OpenGL::GetInstance()->DisableVBO();
}

/////////////////////////////////////////////////////
/// Method: DrawVertexBufferObject
/// Params: None
///
/////////////////////////////////////////////////////
void Emitter::DrawVertexBufferObject( )
{
	int i=0;

	if( m_AllParticlesDisabled )
		return;

	if( !m_VBOInitialised )
	{
		SetupVertexBufferObject();
		return;
	}

	if( m_PointBufferObject !=renderer::INVALID_OBJECT )
	{
		glBindBuffer( GL_ARRAY_BUFFER, m_PointBufferObject );
			
		// Map the buffer object
#ifdef BASE_SUPPORT_OPENGLES		
		math::Vec3 *pMappedBuffer = (math::Vec3 *)glMapBufferOES( GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES );
#else
		math::Vec3 *pMappedBuffer = (math::Vec3 *)glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
#endif //		

		if( pMappedBuffer )
		{
			if( m_UsePoints || m_UseHWPointSprites )
			{
				//std::memcpy( pMappedBuffer, &m_ParticleVertList[0].v, sizeof(ParticleVert)*m_MaxParticles );
				for( i = 0; i < m_MaxParticles; i++ )
				{
					pMappedBuffer[i] = m_ParticleVertList[i].v; 
				}

				glVertexPointer( 3, GL_FLOAT, 0, (char *) 0 );
#ifdef BASE_SUPPORT_OPENGLES				
				glUnmapBufferOES( GL_ARRAY_BUFFER );
#else
				glUnmapBuffer( GL_ARRAY_BUFFER );	
#endif // 
			}
			else
			{
				//std::memcpy( pMappedBuffer, &m_ParticleVertList[0].v, (sizeof(ParticleVert)*m_MaxParticles)*6 );
				for( i = 0; i < m_MaxParticles*6; i++ )
				{
					pMappedBuffer[i] = m_ParticleVertList[i].v; 
				}

				glVertexPointer( 3, GL_FLOAT, 0, VBO_OFFSET(0) );
#ifdef BASE_SUPPORT_OPENGLES	
				glUnmapBufferOES( GL_ARRAY_BUFFER );
#else
				glUnmapBuffer( GL_ARRAY_BUFFER );
#endif //
			}
		}
	}

	if( m_TextureBufferObject2D !=renderer::INVALID_OBJECT )
	{
		glClientActiveTexture( GL_TEXTURE0 );
		renderer::OpenGL::GetInstance()->EnableTextureArray();
			glBindBuffer( GL_ARRAY_BUFFER, m_TextureBufferObject2D );
			glTexCoordPointer( 2, GL_FLOAT, sizeof(ParticleVert), VBO_OFFSET(0) );
	}
	else
		renderer::OpenGL::GetInstance()->DisableTextureArray();

	if( m_ColourBufferObject !=renderer::INVALID_OBJECT )
	{
		renderer::OpenGL::GetInstance()->EnableColourArray();
		glBindBuffer( GL_ARRAY_BUFFER, m_ColourBufferObject );
			
		// Map the buffer object
#ifdef BASE_SUPPORT_OPENGLES	
		math::Vec4Lite *pMappedBuffer = (math::Vec4Lite *)glMapBufferOES( GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES );		
#else
		math::Vec4Lite *pMappedBuffer = (math::Vec4Lite *)glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
#endif //
		
		if( pMappedBuffer )
		{
			if( m_UsePoints || m_UseHWPointSprites )
			{
				for( i = 0; i < m_MaxParticles; i++ )
				{
					pMappedBuffer[i] = m_ParticleVertList[i].col; 
				}

				glColorPointer( 4, GL_UNSIGNED_BYTE, 0, VBO_OFFSET(0) );
#ifdef BASE_SUPPORT_OPENGLES	
				glUnmapBufferOES( GL_ARRAY_BUFFER );
#else
				glUnmapBuffer( GL_ARRAY_BUFFER );
#endif //
			}
			else
			{
				//std::memcpy( pMappedBuffer, &m_ParticleVertList[0].col, (sizeof(ParticleVert)*m_MaxParticles)*6 );
				for( i = 0; i < m_MaxParticles*6; i++ )
				{
					pMappedBuffer[i] = m_ParticleVertList[i].col; 
				}

				glColorPointer( 4, GL_UNSIGNED_BYTE, 0, VBO_OFFSET(0) );
#ifdef BASE_SUPPORT_OPENGLES		
				glUnmapBufferOES( GL_ARRAY_BUFFER );
#else
				glUnmapBuffer( GL_ARRAY_BUFFER );
#endif // 
			}
		}
	}
	else
		renderer::OpenGL::GetInstance()->DisableColourArray();

	if( m_UsePoints || m_UseHWPointSprites )
		glDrawArrays(GL_POINTS, 0, m_MaxParticles);
	else
		glDrawArrays(GL_TRIANGLES, 0, 6*m_MaxParticles);

	renderer::OpenGL::GetInstance()->DisableColourArray();

	glClientActiveTexture( GL_TEXTURE0 );
	renderer::OpenGL::GetInstance()->DisableTextureArray();

	renderer::OpenGL::GetInstance()->DisableVBO();
}

/////////////////////////////////////////////////////
/// Method: DrawVA
/// Params: None
///
/////////////////////////////////////////////////////
void Emitter::DrawVA( )
{

	renderer::OpenGL::GetInstance()->DisableVBO();

	if( m_AllParticlesDisabled )
		return;

	if( m_MaxParticles > 0)
	{
		renderer::OpenGL::GetInstance()->EnableTextureArray();
		renderer::OpenGL::GetInstance()->EnableColourArray();

		if( m_UsePoints || m_UseHWPointSprites )
		{
			glVertexPointer( 3, GL_FLOAT, sizeof(ParticleVert), &m_ParticleVertList[0].v );

			glClientActiveTexture( GL_TEXTURE0 );
				glTexCoordPointer( 2, GL_FLOAT, sizeof( ParticleVert ), &m_ParticleVertList[0].uv );
			
			glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( ParticleVert ), &m_ParticleVertList[0].col );
		}
		else
		{
			glVertexPointer( 3, GL_FLOAT, sizeof( ParticleVert ), &m_ParticleVertList[0].v );

			glClientActiveTexture( GL_TEXTURE0 );
				glTexCoordPointer( 2, GL_FLOAT, sizeof( ParticleVert ), &m_ParticleVertList[0].uv );

			glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( ParticleVert ), &m_ParticleVertList[0].col );
		}

		if( m_UsePoints || m_UseHWPointSprites )
			glDrawArrays(GL_POINTS, 0, m_MaxParticles);
		else
			glDrawArrays(GL_TRIANGLES, 0, 6*m_MaxParticles);

		renderer::OpenGL::GetInstance()->DisableColourArray();

		glClientActiveTexture( GL_TEXTURE0 );
		renderer::OpenGL::GetInstance()->DisableTextureArray();
	}
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void Emitter::DrawDebug( )
{
	glPushMatrix();
		renderer::DrawAABB( m_SystemBoundingBox.vBoxMin, m_SystemBoundingBox.vBoxMax );
	glPopMatrix();
}