
/*===================================================================
	File: WaveGrid.cpp
	Game: H8

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"

#include "Effects/WaveGrid.h"

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
WaveGrid::WaveGrid( const math::Vec3& pos, int gridWidth, int gridDepth, int gridWidthPoints, int gridDepthPoints, float sinAngle, float waveTime )
{
	int i=0, j=0;
	m_AnimatedVertices = 0;
	m_AABB.Reset();

	m_Pos = pos;
	m_GridWidth = gridWidth;
	m_GridDepth = gridDepth;
	m_GridWidthPoints = gridWidthPoints;
	m_GridDepthPoints = gridDepthPoints;
	m_SinAngle = sinAngle;
	m_WaveChangeTime = waveTime;

	m_WidthPointOffset = static_cast<float>(m_GridWidth) / static_cast<float>(m_GridWidthPoints);
	m_DepthPointOffset = static_cast<float>(m_GridDepth) / static_cast<float>(m_GridDepthPoints);

	bVBOInitialised = false;
	nVertexBufferObject =renderer::INVALID_OBJECT;
	nTextureBufferObject2D =renderer::INVALID_OBJECT;
	nNormalBufferObject =renderer::INVALID_OBJECT;

    // Setup flag points
	m_AnimatedVertices = new math::Vec3*[m_GridWidthPoints];
	for( i=0; i < m_GridWidthPoints; i++)
		m_AnimatedVertices[i] = new math::Vec3[m_GridDepthPoints];

    for( i=0; i < m_GridWidthPoints; i++)
    {
        for( j=0; j < m_GridDepthPoints; j++)
        {
            m_AnimatedVertices[i][j].X = static_cast<float>(i) * m_WidthPointOffset;
            m_AnimatedVertices[i][j].Z = static_cast<float>(j) * m_DepthPointOffset;
            
            float yPoints = static_cast<float>(m_GridDepthPoints-1);
            float sinVal = (static_cast<float>(i)*yPoints / m_SinAngle) * 2.0f * math::PI;
            m_AnimatedVertices[i][j].Y = std::sin(sinVal);

			if( m_AnimatedVertices[i][j].X > m_AABB.vBoxMax.X )
				m_AABB.vBoxMax.X = m_AnimatedVertices[i][j].X;
			if( m_AnimatedVertices[i][j].X < m_AABB.vBoxMin.X )
				m_AABB.vBoxMin.X = m_AnimatedVertices[i][j].X;

			if( m_AnimatedVertices[i][j].Y > m_AABB.vBoxMax.Y )
				m_AABB.vBoxMax.Y = m_AnimatedVertices[i][j].Y;
			if( m_AnimatedVertices[i][j].Y < m_AABB.vBoxMin.Y )
				m_AABB.vBoxMin.Y = m_AnimatedVertices[i][j].Y;

			if( m_AnimatedVertices[i][j].Z > m_AABB.vBoxMax.Z )
				m_AABB.vBoxMax.Z = m_AnimatedVertices[i][j].Z;
			if( m_AnimatedVertices[i][j].Z < m_AABB.vBoxMin.Z )
				m_AABB.vBoxMin.Z = m_AnimatedVertices[i][j].Z;
        }
    }

	
	m_AABB.vBoxMin = pos + m_AABB.vBoxMin;
	m_AABB.vBoxMax = pos + m_AABB.vBoxMax;

	m_AABB.vCenter = m_AABB.vBoxMax - ((m_AABB.vBoxMax-m_AABB.vBoxMin)*0.5f); 

    // Allocate space for verts & texture coordinates
    m_StripVertexCount = ((m_GridDepthPoints - 1) * 2);
	m_Vertices = new math::Vec3[m_StripVertexCount];
	m_TexCoords = new GLfloat[m_StripVertexCount*4];

	m_WaveTime = 0.0f;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
WaveGrid::~WaveGrid( )
{
	if( m_TexCoords != 0 )
	{
		delete[] m_TexCoords;
		m_TexCoords = 0;
	}

	if( m_Vertices != 0 )
	{
		delete[] m_Vertices;
		m_Vertices = 0;
	}

	int i=0;
	for( i=0; i < m_GridWidthPoints; i++)
	{
		if( m_AnimatedVertices[i] != 0 )
			delete[] m_AnimatedVertices[i];
	}
	if( m_AnimatedVertices != 0 )
	{
		delete[] m_AnimatedVertices;
		m_AnimatedVertices = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void WaveGrid::Draw( void )
{
	if( !renderer::OpenGL::GetInstance()->AABBInFrustum( m_AABB ) )
		return;

	if( core::app::GetUseVertexArrays() ||
		!renderer::bExtVertexBufferObjects)
		DrawVA();
	else
		DrawVertexBufferObject();
}

/////////////////////////////////////////////////////
/// Method: SetupVertexBufferObject
/// Params: None
///
/////////////////////////////////////////////////////
void WaveGrid::SetupVertexBufferObject( void )
{
	if( core::app::GetUseVertexArrays() ||
		!renderer::bExtVertexBufferObjects)
		return;

	if( !bVBOInitialised )
	{
		glGenBuffers( 1, &nVertexBufferObject );						
		glBindBuffer( GL_ARRAY_BUFFER, nVertexBufferObject );	
		glBufferData( GL_ARRAY_BUFFER, m_StripVertexCount*sizeof(math::Vec3), m_Vertices, GL_DYNAMIC_DRAW );
		glVertexPointer( 3, GL_FLOAT, 0, m_Vertices );
			
		GL_CHECK			

		glClientActiveTexture( GL_TEXTURE0 );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			
		//  default diffuse
		glGenBuffers( 1, &nTextureBufferObject2D );						
		glBindBuffer( GL_ARRAY_BUFFER, nTextureBufferObject2D );	

		glBufferData( GL_ARRAY_BUFFER, m_StripVertexCount*(sizeof(GLfloat)*4), m_TexCoords, GL_DYNAMIC_DRAW );
		glTexCoordPointer( 2, GL_FLOAT, 0, m_TexCoords );
			
		GL_CHECK		

		/*if( bHasNormals )
		{
			renderer::OpenGL::GetInstance()->EnableNormalArray();
			
			glGenBuffers( 1, &nNormalBufferObject );						
			glBindBuffer( GL_ARRAY_BUFFER, nNormalBufferObject );

			glBufferData( GL_ARRAY_BUFFER, nTotalVertexCount*sizeof(math::Vec3), pNormals, GL_STATIC_DRAW );
			glNormalPointer( GL_FLOAT, sizeof( math::Vec3 ), pNormals );
			
			GL_CHECK			
		}*/

		
		bVBOInitialised = true;
	}

	//////////////////////////////////////
	
	glClientActiveTexture( GL_TEXTURE0 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		
	renderer::OpenGL::GetInstance()->DisableVBO();
}

/////////////////////////////////////////////////////
/// Method: DrawVertexBufferObject
/// Params: None
///
/////////////////////////////////////////////////////
void WaveGrid::DrawVertexBufferObject( void )
{
	if( !bVBOInitialised )
	{
		SetupVertexBufferObject();
		return;
	}

	int i = 0, j = 0;	
    int vertexCounter = 0;
    int texCoordCounter = 0;
	//math::Vec3 *pMappedVertBuffer = 0;
	//GLfloat* pMappedTexBuffer = 0;

    for ( i=0; i<m_GridWidthPoints-1; i++)
    {		
		for (j=0; j<m_GridDepthPoints-1; j++)
		{	
/*
#ifdef BASE_SUPPORT_OPENGLES
			// Map the buffer object
			pMappedVertBuffer = (math::Vec3 *)glMapBufferOES( GL_ARRAY_BUFFER, GL_WRITE_ONLY_OES );

			if( pMappedVertBuffer )
			{
				for( i = 0; i < nNumVertices; i++ )
				{
					pMappedBuffer[i] = *ppVertexPositionPointer[i]; 
				}
				glVertexPointer( 3, GL_FLOAT, 0, (char *) 0 );
				glUnmapBufferOES( GL_ARRAY_BUFFER );
			}
#else

			// bind and map vert buffer
			glBindBuffer( GL_ARRAY_BUFFER, nVertexBufferObject );
			pMappedVertBuffer = (math::Vec3 *)glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
			glBindBuffer( GL_ARRAY_BUFFER, nTextureBufferObject2D );
			pMappedTexBuffer = (GLfloat*)glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );

			pMappedVertBuffer[vertexCounter++] = m_AnimatedVertices[i][j];
			pMappedVertBuffer[vertexCounter++] = m_AnimatedVertices[i+1][j];

			// Calculate the texture coordinates for the two triangles
			pMappedTexBuffer[texCoordCounter++] = static_cast<GLfloat>(i) * 1.0f / static_cast<GLfloat>(m_GridWidth);
			pMappedTexBuffer[texCoordCounter++] = 1.0f - (static_cast<GLfloat>(j) * 1.0f / static_cast<GLfloat>(m_GridDepth));
			pMappedTexBuffer[texCoordCounter++] = static_cast<GLfloat>(i+1) * 1.0f / static_cast<GLfloat>(m_GridWidth);
			pMappedTexBuffer[texCoordCounter++] = 1.0f - (static_cast<GLfloat>(j) * 1.0f / static_cast<GLfloat>(m_GridDepth));

			glBindBuffer( GL_ARRAY_BUFFER, nVertexBufferObject );
			glUnmapBuffer( GL_ARRAY_BUFFER );
			glBindBuffer( GL_ARRAY_BUFFER, nTextureBufferObject2D );
			glUnmapBuffer( GL_ARRAY_BUFFER );
#endif
*/
			m_Vertices[vertexCounter++] = m_AnimatedVertices[i][j];
            m_Vertices[vertexCounter++] = m_AnimatedVertices[i+1][j];
            //normals[normalCounter++] = flagVertexNormals[i][j];
            //normals[normalCounter++] = flagVertexNormals[i+1][j];
            
            // Calculate the texture coordinates for the two triangles
            m_TexCoords[texCoordCounter++] = static_cast<GLfloat>(i) * 1.0f / static_cast<GLfloat>(m_GridWidthPoints);
            m_TexCoords[texCoordCounter++] = 1.0f - (static_cast<GLfloat>(j) * 1.0f / static_cast<GLfloat>(m_GridDepthPoints));
            m_TexCoords[texCoordCounter++] = static_cast<GLfloat>(i+1) * 1.0f / static_cast<GLfloat>(m_GridWidthPoints);
            m_TexCoords[texCoordCounter++] = 1.0f - (static_cast<GLfloat>(j) * 1.0f / static_cast<GLfloat>(m_GridDepthPoints));
		}

		glBindBuffer( GL_ARRAY_BUFFER, nVertexBufferObject );
		glBufferData( GL_ARRAY_BUFFER, m_StripVertexCount*sizeof(math::Vec3), m_Vertices, GL_DYNAMIC_DRAW );
		glVertexPointer( 3, GL_FLOAT, 0, 0 );

		glBindBuffer( GL_ARRAY_BUFFER, nTextureBufferObject2D );
		glBufferData( GL_ARRAY_BUFFER, m_StripVertexCount*(sizeof(GLfloat)*4), m_TexCoords, GL_DYNAMIC_DRAW );
		glTexCoordPointer( 2, GL_FLOAT, 0, 0 );

		glDrawArrays( GL_TRIANGLE_STRIP, 0, m_StripVertexCount );

		vertexCounter = 0;
		texCoordCounter = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: DrawVA
/// Params: None
///
/////////////////////////////////////////////////////
void WaveGrid::DrawVA( void )
{
	int i=0, j=0;
    int vertexCounter = 0;
    int texCoordCounter = 0;
    //int normalCounter = 0;

    for ( i=0; i<m_GridWidthPoints-1; i++)
    {
        for (j=0; j<m_GridDepthPoints-1; j++)
        {
            m_Vertices[vertexCounter++] = m_AnimatedVertices[i][j];
            m_Vertices[vertexCounter++] = m_AnimatedVertices[i+1][j];
            //normals[normalCounter++] = flagVertexNormals[i][j];
            //normals[normalCounter++] = flagVertexNormals[i+1][j];
            
            // Calculate the texture coordinates for the two triangles
            m_TexCoords[texCoordCounter++] = static_cast<GLfloat>(i) * 1.0f / static_cast<GLfloat>(m_GridWidthPoints);
            m_TexCoords[texCoordCounter++] = 1.0f - (static_cast<GLfloat>(j) * 1.0f / static_cast<GLfloat>(m_GridDepthPoints));
            m_TexCoords[texCoordCounter++] = static_cast<GLfloat>(i+1) * 1.0f / static_cast<GLfloat>(m_GridWidthPoints);
            m_TexCoords[texCoordCounter++] = 1.0f - (static_cast<GLfloat>(j) * 1.0f / static_cast<GLfloat>(m_GridDepthPoints));
        }
        
        glVertexPointer(3, GL_FLOAT, 0, m_Vertices); 
        glTexCoordPointer(2, GL_FLOAT, 0, m_TexCoords);
        //glNormalPointer(GL_FLOAT, 0, normals);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, m_StripVertexCount);
        vertexCounter = 0;
        texCoordCounter = 0;
        //normalCounter = 0;
    }
}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: 
///
/////////////////////////////////////////////////////
void WaveGrid::DrawDebug( void )
{
	renderer::DrawAABB( m_AABB.vBoxMin, m_AABB.vBoxMax );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void WaveGrid::Update( float deltaTime )
{	
	if( !renderer::OpenGL::GetInstance()->AABBInFrustum( m_AABB ) )
		return;

	m_WaveTime += deltaTime;

	if( m_WaveTime > m_WaveChangeTime )
	{
		m_WaveTime = 0.0f;
		int i=0,j=0;

		// Move the flag m_Vertices along sine wave
		for ( j=0 ; j < m_GridDepthPoints-1; j++)
		{
			float vertexWrap = m_AnimatedVertices[m_GridWidthPoints-1][j].Y;
			//Vertex3D normalWrap = flagVertexNormals[m_GridWidthPoints-1][j];
			for ( i=m_GridWidthPoints-1; i >= 1; i--)
			{
				m_AnimatedVertices[i][j].Y = m_AnimatedVertices[i-1][j].Y;
				//flagVertexNormals[i][j] = flagVertexNormals[i-1][y];
			}
			m_AnimatedVertices[0][j].Y = vertexWrap;
			//flagVertexNormals[0][j] = normalWrap;
		}
	}
}
