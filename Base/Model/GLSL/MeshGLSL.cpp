
/*===================================================================
	File: MeshGLSL.cpp
	Library: ModelLoaderLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_OPENGL_GLSL

#include "CoreBase.h"

#include "glm/gtc/matrix_transform.hpp"

#include <cmath>

#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"

#include "Model/ModelCommon.h"
#include "Model/Model.h"
#include "Model/GLSL/MeshGLSL.h"
#include "Model/GLSL/SkinMeshGLSL.h"

using mdl::Mesh;

/////////////////////////////////////////////////////
/// Function: MeshSort
/// Params: [in]pMesh1, [in]mesh2
///
/////////////////////////////////////////////////////
int mdl::MeshSort( const void *pMesh1, const void *pMesh2 )
{
	mdl::Mesh *pFirst = (mdl::Mesh*) pMesh1;
	mdl::Mesh *pSecond = (mdl::Mesh*) pMesh2;

	return (pFirst->nMeshIndex <= pSecond->nMeshIndex) ? -1 : 1;
}

/////////////////////////////////////////////////////
/// Function: MeshSortByDepth
/// Params: [in]pMesh1, [in]mesh2
///
/////////////////////////////////////////////////////
int mdl::MeshSortByDepth( const void *pMesh1, const void *pMesh2 )
{
	mdl::Mesh *pFirst = (mdl::Mesh*) pMesh1;
	mdl::Mesh *pSecond = (mdl::Mesh*) pMesh2;

	if(pFirst->sceneDepth > pSecond->sceneDepth)
		return(-1);
	if(pFirst->sceneDepth < pSecond->sceneDepth)
		return(1);

	return(0);
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
Mesh::Mesh()
{
	Initialise();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
Mesh::~Mesh()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void Mesh::Initialise( void )
{
	int i=0;

	nGeneralFlags				= 0;
	nDataFlushFlags				= 0;

	nInterleavedBufferObject	= renderer::INVALID_OBJECT;
	nVertexBufferObject			= renderer::INVALID_OBJECT;
	nTextureBufferObject2D		= renderer::INVALID_OBJECT;
	nNormalBufferObject			= renderer::INVALID_OBJECT;
	nElementBufferObject		= renderer::INVALID_OBJECT;

	nActiveProgram				= renderer::INVALID_OBJECT;
	nVertexAttribLocation		= -1;
	nNormalAttribLocation		= -1;
	nTexCoordsAttribLocation	= -1;
	nTangentAttribLocation		= -1;

	for( i=0; i < MODEL_MAX_USEABLE_TEXUNITS; ++i )
		nTexSamplerUniform[i]	= -1;

	bVBOInitialised				= false;

	bHasVertices		= true;
	bHasTextureCoords	= true;
	bHasNormals			= true;
	bHasTangents		= false;
	bSortMeshes			= false;

	nNumVertexColours	= 0;
	nNumVertices		= 0;
	nNumNormals			= 0;
	nNumTextureCoords	= 0;
	nNumMaterials		= 0;

	pInterleavedData = 0;

	nNumTexAnimationFrames = 0;
	nTexAnimInterleavedBufferObject = 0;
	pTexAnimInterleavedData = 0;
	nOriginalInterleavedBufferObject = renderer::INVALID_OBJECT;

	pMaterials			= 0;

	nNumMaterialTriangleLists = 0;
	pMaterialIndexList	= 0;
	pTriPerMaterialList	= 0;

	nMeshId				= 0;
	nMeshIndex			= 0;

	// defaults
	savedDepthBlendAlphaState.bDepthState	= true;
	savedDepthBlendAlphaState.eDepthFunc	= GL_LESS;
	savedDepthBlendAlphaState.bBlendState	= false;
	savedDepthBlendAlphaState.eBlendSrc		= GL_SRC_ALPHA;
	savedDepthBlendAlphaState.eBlendDest	= GL_ONE_MINUS_SRC_ALPHA;
	savedDepthBlendAlphaState.bAlphaState	= false;
	savedDepthBlendAlphaState.eAlphaFunc	= GL_ALWAYS;
	savedDepthBlendAlphaState.fAlphaClamp	= 0.0f;

	savedCullFaceState.bCullState			= true;
	savedCullFaceState.eCullFace			= GL_BACK;

	savedFrontFaceWinding					= GL_CCW;

	// defaults
	meshDepthBlendAlphaState.bDepthState	= true;
	meshDepthBlendAlphaState.eDepthFunc		= GL_LESS;
	meshDepthBlendAlphaState.bBlendState	= false;
	meshDepthBlendAlphaState.eBlendSrc		= GL_SRC_ALPHA;
	meshDepthBlendAlphaState.eBlendDest		= GL_ONE_MINUS_SRC_ALPHA;
	meshDepthBlendAlphaState.bAlphaState	= false;
	meshDepthBlendAlphaState.eAlphaFunc		= GL_ALWAYS;
	meshDepthBlendAlphaState.fAlphaClamp	= 0.0f;

	meshCullFaceState.bCullState			= true;
	meshCullFaceState.eCullFace				= GL_BACK;

	meshFrontFaceWinding					= GL_CCW;

	meshCullFaceState.bCullState			= true;
	meshCullFaceState.eCullFace				= GL_BACK;

	meshSort								= false;

	meshSimpleColourFlag					= false;
	meshSimpleColour						= math::Vec4Lite( 255,255,255,255 );

	meshTranslate							= false;
	meshTranslation							= math::Vec3( 0.0f, 0.0f, 0.0f );

	meshRotate								= false;
	meshRotation							= math::Vec3( 0.0f, 0.0f, 0.0f );

	meshScale								= false;
	meshScaleFactors						= math::Vec3( 1.0f, 1.0f, 1.0f );

	numTextureAnimationFrames				= 0;
	currentTextureAnimationFrame			= 0;
	currentTextureAnimationTimer			= 0.0f;
	pTextureAnimationData					= 0;
	originalTextureId						= renderer::INVALID_OBJECT;
	
	// bounding defaults
	meshAABB.Reset();
	meshSphere.vCenterPoint = math::Vec3( 0.0f, 0.0f, 0.0f );
	meshSphere.fRadius = 1.0f;

	bIsSelected			= false;

	nTotalPolyCount		= 0;
	nTotalVertexCount	= 0;
	nTotalTriangleCount	= 0;

	bDraw = true;
	bValidLoad = false;

	sceneDepth = 0.0f;

	textureAnimUpdated = false;
}

/////////////////////////////////////////////////////
/// Method: SetActiveProgram
/// Params: [in]programId
///
/////////////////////////////////////////////////////
void Mesh::SetActiveProgram( GLuint programId )
{
	if( nActiveProgram != programId )
	{
		nActiveProgram = programId;

		// reload vbo?
		if( bVBOInitialised )
		{
			bVBOInitialised = false;
			SetupVertexBufferObject();
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void Mesh::Draw( void )
{
	if( core::app::GetUseVertexArrays() )
		DrawVA();
	else
		DrawVertexBufferObject();

	textureAnimUpdated = false;
}

/////////////////////////////////////////////////////
/// Method: SetupVertexBufferObject
/// Params: None
///
/////////////////////////////////////////////////////
void Mesh::SetupVertexBufferObject( void )
{
	if( core::app::GetUseVertexArrays() )
		return;

	if( nActiveProgram == renderer::INVALID_OBJECT ||
		nActiveProgram == 0 )
		return;

	GLuint prevProg = renderer::OpenGL::GetInstance()->GetCurrentProgram();
	
	if( !bVBOInitialised )
	{
		renderer::OpenGL::GetInstance()->UseProgram(nActiveProgram);

		nVertexAttribLocation = glGetAttribLocation( nActiveProgram, "base_v" );
		nTexCoordsAttribLocation = glGetAttribLocation( nActiveProgram, "base_uv0" );
		nNormalAttribLocation = glGetAttribLocation( nActiveProgram, "base_n" );

		if( bHasTangents )
			nTangentAttribLocation = glGetAttribLocation( nActiveProgram, "base_t" );

		nTexSamplerUniform[0] = glGetUniformLocation( nActiveProgram, "texUnit0" ); 
		if( nTexSamplerUniform[0] != -1 )
			glUniform1i( nTexSamplerUniform[0], 0 );

		nTexSamplerUniform[1] = glGetUniformLocation( nActiveProgram, "texUnit1" ); 
		if( nTexSamplerUniform[1] != -1 )
			glUniform1i( nTexSamplerUniform[1], 1 );

		nTexSamplerUniform[2] = glGetUniformLocation( nActiveProgram, "texUnit2" ); 
		if( nTexSamplerUniform[2] != -1 )
			glUniform1i( nTexSamplerUniform[2], 2 );

		nTexSamplerUniform[3] = glGetUniformLocation( nActiveProgram, "texUnit3" ); 
		if( nTexSamplerUniform[3] != -1 )
			glUniform1i( nTexSamplerUniform[3], 3 );

		if( nInterleavedBufferObject == renderer::INVALID_OBJECT )
			glGenBuffers( 1, &nInterleavedBufferObject );						

		glBindBuffer( GL_ARRAY_BUFFER, nInterleavedBufferObject );
		glBufferData( GL_ARRAY_BUFFER, nTotalVertexCount*(sizeof(InterleavedData)), &pInterleavedData[0].v, GL_STATIC_DRAW );
			
		GL_CHECK

		// store for use with texture anims
		nOriginalInterleavedBufferObject = nInterleavedBufferObject;

		bVBOInitialised = true;
	}

	// FIXME: this can be deleted if the shader doesn't change
	/*if( pInterleavedData )
	{
		delete[] pInterleavedData;
		pInterleavedData = 0;
	}*/

	GL_CHECK

	renderer::OpenGL::GetInstance()->UseProgram(prevProg);
	
	renderer::OpenGL::GetInstance()->DisableVBO();
}

/////////////////////////////////////////////////////
/// Method: DrawVertexBufferObject
/// Params: None
///
/////////////////////////////////////////////////////
void Mesh::DrawVertexBufferObject( void )
{
	if( !bValidLoad || !bDraw )
		return;

	if( !bVBOInitialised )
	{
		SetupVertexBufferObject();
		return;
	}

	unsigned int i = 0;
	int j = 0;	

	int startIndices, endIndices;

	startIndices = endIndices = 0;

	if( nNumMaterialTriangleLists )
	{

		if( nInterleavedBufferObject != renderer::INVALID_OBJECT )
			glBindBuffer( GL_ARRAY_BUFFER, nInterleavedBufferObject );

		if( bHasVertices && 
			nVertexAttribLocation != -1 )
		{
			glEnableVertexAttribArray( nVertexAttribLocation );
			glVertexAttribPointer( nVertexAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(InterleavedData), VBO_OFFSET(0) );
			
			GL_CHECK	
		}

		if( bHasTextureCoords &&
			nTexCoordsAttribLocation != -1)
		{
			// use the same uv coords if coords are not generated

			// optional
			for( i = 0; i < nNumMaterialTriangleLists; i++ )
			{
				// only if enabled
				if( pMaterials[i].texUnit[j].bEnabled  )
				{
					glEnableVertexAttribArray( nTexCoordsAttribLocation );
					glVertexAttribPointer( nTexCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(InterleavedData), VBO_OFFSET(3*sizeof(float)) );
				}
			}
			
			GL_CHECK
		}

		if( bHasNormals &&
			nNormalAttribLocation != -1 )
		{
			// move offset passed 3 verts and 2 uvs
			glEnableVertexAttribArray( nNormalAttribLocation );
			glVertexAttribPointer( nNormalAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(InterleavedData), VBO_OFFSET(5*sizeof(float)) );

			GL_CHECK			
		}

		if( nTangentAttribLocation != -1 )
		{
			// move offset passed 3 verts, 2 uvs and 3 normals
			glEnableVertexAttribArray( nTangentAttribLocation );
			glVertexAttribPointer( nTangentAttribLocation, 4, GL_FLOAT, GL_FALSE, sizeof(InterleavedData), VBO_OFFSET(8*sizeof(float)) );
		}
		
		for( i = 0; i < nNumMaterialTriangleLists; i++ )
		{
			endIndices = ( (3*pTriPerMaterialList[i]) )+startIndices;

			renderer::OpenGL::GetInstance()->SetMaterialAmbient( pMaterials[i].Ambient );
			renderer::OpenGL::GetInstance()->SetMaterialDiffuse( pMaterials[i].Diffuse );

			renderer::OpenGL::GetInstance()->SetMaterialSpecular( pMaterials[i].Specular );
			renderer::OpenGL::GetInstance()->SetMaterialEmission( pMaterials[i].Emissive );
			renderer::OpenGL::GetInstance()->SetMaterialShininess( pMaterials[i].Shininess );

			// only if enabled
			for( j = MODEL_MAX_USEABLE_TEXUNITS-1; j >= 0; j-- )
			{
				if( pMaterials[i].texUnit[j].bEnabled )
					SetMaterialData( j, &pMaterials[i] );
			}
          
			glDrawArrays( GL_TRIANGLES, startIndices, (3*pTriPerMaterialList[i]) );

			startIndices = endIndices;

			GL_CHECK
		}
	}

	if( bHasVertices && 
		nVertexAttribLocation != -1 )
		glDisableVertexAttribArray( nVertexAttribLocation );

	if( bHasTextureCoords && 
		nTexCoordsAttribLocation != -1 )
		glDisableVertexAttribArray( nTexCoordsAttribLocation );

	if( bHasNormals &&
		nNormalAttribLocation != -1 )
		glDisableVertexAttribArray( nNormalAttribLocation );

	if( nTangentAttribLocation != -1 )
		glDisableVertexAttribArray( nTangentAttribLocation );

	renderer::OpenGL::GetInstance()->DisableVBO();
}

/////////////////////////////////////////////////////
/// Method: DrawVA
/// Params: None
///
/////////////////////////////////////////////////////
void Mesh::DrawVA( void )
{
	if( !bValidLoad || !bDraw )
		return;

	if( nActiveProgram == renderer::INVALID_OBJECT ||
		nActiveProgram == 0 )
		return;

	renderer::OpenGL::GetInstance()->DisableVBO();

	renderer::OpenGL::GetInstance()->UseProgram(nActiveProgram);

	nVertexAttribLocation = glGetAttribLocation( nActiveProgram, "base_v" );
	nTexCoordsAttribLocation = glGetAttribLocation( nActiveProgram, "base_uv0" );
	nNormalAttribLocation = glGetAttribLocation( nActiveProgram, "base_n" );

	if( bHasTangents )
		nTangentAttribLocation = glGetAttribLocation( nActiveProgram, "base_t" );

	nTexSamplerUniform[0] = glGetUniformLocation( nActiveProgram, "texUnit0" ); 
	if( nTexSamplerUniform[0] != -1 )
		glUniform1i( nTexSamplerUniform[0], 0 );

	nTexSamplerUniform[1] = glGetUniformLocation( nActiveProgram, "texUnit1" ); 
	if( nTexSamplerUniform[1] != -1 )
		glUniform1i( nTexSamplerUniform[1], 1 );

	nTexSamplerUniform[2] = glGetUniformLocation( nActiveProgram, "texUnit2" ); 
	if( nTexSamplerUniform[2] != -1 )
		glUniform1i( nTexSamplerUniform[2], 2 );

	nTexSamplerUniform[3] = glGetUniformLocation( nActiveProgram, "texUnit3" ); 
	if( nTexSamplerUniform[3] != -1 )
		glUniform1i( nTexSamplerUniform[3], 3 );

	unsigned int i = 0;
	int j = 0;	
	int startIndices, endIndices;

	startIndices = endIndices = 0;

	if( nNumMaterialTriangleLists )
	{
		if( bHasVertices && 
			nVertexAttribLocation != -1 )
		{
			glEnableVertexAttribArray( nVertexAttribLocation );
			glVertexAttribPointer( nVertexAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(InterleavedData), &pInterleavedData[0].v  );
			
			GL_CHECK	
		}

		if( bHasTextureCoords &&
			nTexCoordsAttribLocation != -1)
		{
			// use the same uv coords if coords are not generated

			// optional
			for( i = 0; i < nNumMaterialTriangleLists; i++ )
			{
				// only if enabled
				for( j = MODEL_MAX_USEABLE_TEXUNITS-1; j >= 0; j-- )
				{
					if( pMaterials[i].texUnit[j].bEnabled  )// && j < MODEL_MAX_USEABLE_TEXUNITS
					{
						glEnableVertexAttribArray( nTexCoordsAttribLocation );
						glVertexAttribPointer( nTexCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(InterleavedData), &pInterleavedData[0].uv );
					}
				}
			}
			
			GL_CHECK		
		}

		if( bHasNormals &&
			nNormalAttribLocation != -1 )
		{
			// move offset passed 3 verts and 2 uvs
			glEnableVertexAttribArray( nNormalAttribLocation );
			glVertexAttribPointer( nNormalAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(InterleavedData), &pInterleavedData[0].n );
		
			GL_CHECK			
		}
		

		if( nTangentAttribLocation != -1 )
		{
			glEnableVertexAttribArray( nTangentAttribLocation );
			glVertexAttribPointer( nTangentAttribLocation, 4, GL_FLOAT, GL_FALSE, sizeof(InterleavedData), &pInterleavedData[0].t );
		
			GL_CHECK		
		}

		for( i = 0; i < nNumMaterialTriangleLists; i++ )
		{
			endIndices = ( (3*pTriPerMaterialList[i]) )+startIndices;
			
			renderer::OpenGL::GetInstance()->SetMaterialAmbient( pMaterials[i].Ambient );
			renderer::OpenGL::GetInstance()->SetMaterialDiffuse( pMaterials[i].Diffuse );

			renderer::OpenGL::GetInstance()->SetMaterialSpecular( pMaterials[i].Specular );
			renderer::OpenGL::GetInstance()->SetMaterialEmission( pMaterials[i].Emissive );
			renderer::OpenGL::GetInstance()->SetMaterialShininess( pMaterials[i].Shininess );

			for( j = MODEL_MAX_USEABLE_TEXUNITS-1; j >= 0; j-- )
			{	
				// only if enabled
				if( pMaterials[i].texUnit[j].bEnabled )
					SetMaterialData( j, &pMaterials[i] );
			}
			          
            glDrawArrays( GL_TRIANGLES, startIndices, (3*pTriPerMaterialList[i]) );

			startIndices = endIndices;
			
			GL_CHECK
		}
	}

	if( bHasVertices && 
		nVertexAttribLocation != -1 )
		glDisableVertexAttribArray( nVertexAttribLocation );

	if( bHasTextureCoords && 
		nTexCoordsAttribLocation != -1 )
		glDisableVertexAttribArray( nTexCoordsAttribLocation );

	if( bHasNormals &&
		nNormalAttribLocation != -1 )
		glDisableVertexAttribArray( nNormalAttribLocation );

	if( nTangentAttribLocation != -1 )
		glDisableVertexAttribArray( nTangentAttribLocation );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void Mesh::Update( float deltaTime )
{
	unsigned int i=0;

	if( textureAnimUpdated )
		return;

	if( pTextureAnimationData != 0 )
	{
		if( numTextureAnimationFrames > 0 )
		{
			textureAnimUpdated = true;

			currentTextureAnimationTimer += deltaTime;

			if( currentTextureAnimationTimer > pTextureAnimationData[currentTextureAnimationFrame].time )
			{
				currentTextureAnimationFrame++;
				if( currentTextureAnimationFrame >= numTextureAnimationFrames )
					currentTextureAnimationFrame = 0;

				// set the texture
				for( i = 0; i < nNumMaterialTriangleLists; i++ )
				{
					// only if enabled
					//if( pMaterials[i].texUnit[0].bEnabled  )
					{
						// set the first frame
						pMaterials[i].texUnit[0].bEnabled = true;
						pMaterials[i].texUnit[0].nTextureID = pTextureAnimationData[currentTextureAnimationFrame].textureId;
					}
				}

				// reset time
				currentTextureAnimationTimer = 0.0f;
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SetTextureAnimation
/// Params: None
///
/////////////////////////////////////////////////////
void Mesh::SetTextureAnimation( int numFrames, mdl::TextureAnimationBlock::AnimationData* anim )
{
	numTextureAnimationFrames = numFrames;
	pTextureAnimationData = anim;

	int i=0, j=0;

	for( i = 0; i < (int)nNumMaterialTriangleLists; i++ )
	{
		// only if enabled
		if( pMaterials[i].texUnit[0].bEnabled  )
		{
			for( j=0; j < numFrames; ++j )
			{
				pTextureAnimationData[j].originalTextureId = pMaterials[i].texUnit[0].nTextureID;
			}
		}
		else
		{
			// set the first frame
			pMaterials[i].texUnit[0].bEnabled = true;	
		}
		
		pMaterials[i].texUnit[0].nTextureID = pTextureAnimationData[0].textureId;
	}

	currentTextureAnimationTimer = 0.0f;
	currentTextureAnimationFrame = 0;
}

/////////////////////////////////////////////////////
/// Method: CalculateBounds
/// Params: None
///
/////////////////////////////////////////////////////
void Mesh::CalculateBounds( void )
{
	// doesn't have any verts
	if( !bHasVertices )
		return;

	unsigned int i = 0;

	//meshAABB.Reset();
	meshSphere.fRadius = 0.0f;

	float fNewRadius = 0.0f;

	// work out the bound box and sphere
	for( i = 0; i < nNumVertices; i++ )
	{
		// BOUNDING BOX
		if( pInterleavedData[i].v[0] < meshAABB.vBoxMin.X )
			meshAABB.vBoxMin.X = pInterleavedData[i].v[0];

		if( pInterleavedData[i].v[1] < meshAABB.vBoxMin.Y )
			meshAABB.vBoxMin.Y = pInterleavedData[i].v[1];

		if( pInterleavedData[i].v[2] < meshAABB.vBoxMin.Z )
			meshAABB.vBoxMin.Z = pInterleavedData[i].v[2];


		if( pInterleavedData[i].v[0] > meshAABB.vBoxMax.X )
			meshAABB.vBoxMax.X  = pInterleavedData[i].v[0] ;

		if( pInterleavedData[i].v[1] > meshAABB.vBoxMax.Y  )
			meshAABB.vBoxMax.Y = pInterleavedData[i].v[1];

		if( pInterleavedData[i].v[2] > meshAABB.vBoxMax.Z )
			meshAABB.vBoxMax.Z = pInterleavedData[i].v[2];
	}

	//meshAABB.vCenter = (meshAABB.vBoxMax-meshAABB.vBoxMin)*0.5f;

	// BOUNDING SPHERE
	// get a center point based off the box size above
	meshSphere.vCenterPoint.X = meshAABB.vBoxMax.X - ((meshAABB.vBoxMax.X - meshAABB.vBoxMin.X)*0.5f);
	meshSphere.vCenterPoint.Y = meshAABB.vBoxMax.Y - ((meshAABB.vBoxMax.Y - meshAABB.vBoxMin.Y)*0.5f);
	meshSphere.vCenterPoint.Z = meshAABB.vBoxMax.Z - ((meshAABB.vBoxMax.Z - meshAABB.vBoxMin.Z)*0.5f);

	// line lengths (0.75 takes away the over safe full size radius and wraps it around the box nicely)
	float fLineX = (meshAABB.vBoxMax.X - meshAABB.vBoxMin.X)*0.75f;
	float fLineY = (meshAABB.vBoxMax.Y - meshAABB.vBoxMin.Y)*0.75f;
	float fLineZ = (meshAABB.vBoxMax.Z - meshAABB.vBoxMin.Z)*0.75f;

	// get the SMALLEST difference to get as close to the mesh edge
	if( fLineX > fNewRadius )
		fNewRadius = fLineX;	

	if( fLineY > fNewRadius )
		fNewRadius = fLineY;
	
	if( fLineZ > fNewRadius )
		fNewRadius = fLineZ;

	meshSphere.fRadius = fNewRadius;
}

/////////////////////////////////////////////////////
/// Method: DrawBoundingBox
/// Params: None
///
/////////////////////////////////////////////////////
void Mesh::DrawBoundingBox( void )
{
	renderer::DrawAABB( meshAABB.vBoxMin, meshAABB.vBoxMax );
}

/////////////////////////////////////////////////////
/// Method: DrawBoundingSphere
/// Params: None
///
/////////////////////////////////////////////////////
void Mesh::DrawBoundingSphere( void )
{
	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();
	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();

	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );

	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate( modelMatrix, glm::vec3(meshSphere.vCenterPoint.X, meshSphere.vCenterPoint.Y, meshSphere.vCenterPoint.Z) );
	renderer::OpenGL::GetInstance()->SetModelMatrix(modelMatrix);
	renderer::DrawSphere( meshSphere.fRadius );

	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();

	if( lightState )
		renderer::OpenGL::GetInstance()->EnableLighting();
}

/////////////////////////////////////////////////////
/// Method: FlushData
/// Params: None
///
/////////////////////////////////////////////////////
void Mesh::FlushData( void )
{
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void Mesh::Release( void )
{
	Shutdown();

	bDraw = true;
	bValidLoad = false;
}

/////////////////////////////////////////////////////
/// Method: Shutdown
/// Params: None
///
/////////////////////////////////////////////////////
int Mesh::Shutdown( void )
{
	unsigned int i = 0;

	// delete buffer objects
	if( nInterleavedBufferObject != renderer::INVALID_OBJECT )
	{
		glDeleteBuffers( 1, &nInterleavedBufferObject );
		nInterleavedBufferObject = renderer::INVALID_OBJECT;
	}

	for( i=0; i < (unsigned int)nNumTexAnimationFrames; ++i )
	{
		if( nTexAnimInterleavedBufferObject != 0 )
		{
			if( nTexAnimInterleavedBufferObject[i] != renderer::INVALID_OBJECT )
			{
				glDeleteBuffers( 1, &nTexAnimInterleavedBufferObject[i] );
				nTexAnimInterleavedBufferObject[i] = renderer::INVALID_OBJECT;
			}
		}

		if( pTexAnimInterleavedData != 0 )
		{
			if( pTexAnimInterleavedData[i] != 0 )
				delete[] pTexAnimInterleavedData[i];
		}
	}
	if( nTexAnimInterleavedBufferObject != 0 )
		delete[] nTexAnimInterleavedBufferObject;
	
	if( pTexAnimInterleavedData != 0 )
		delete[] pTexAnimInterleavedData;

	if( nVertexBufferObject != renderer::INVALID_OBJECT )
	{
		glDeleteBuffers( 1, &nVertexBufferObject );
		nVertexBufferObject = renderer::INVALID_OBJECT;
	}
	if( nTextureBufferObject2D != renderer::INVALID_OBJECT  )
	{
		glDeleteBuffers( 1, &nTextureBufferObject2D );
		nTextureBufferObject2D = renderer::INVALID_OBJECT;
	}
	if( nNormalBufferObject != renderer::INVALID_OBJECT )
	{
		glDeleteBuffers( 1, &nNormalBufferObject );
		nNormalBufferObject = renderer::INVALID_OBJECT;
	}
	if( nElementBufferObject != renderer::INVALID_OBJECT )
	{
		glDeleteBuffers( 1, &nElementBufferObject );
		nElementBufferObject = renderer::INVALID_OBJECT;
	}

	// delete data
	if( pInterleavedData )
	{
		delete[] pInterleavedData;
		pInterleavedData = 0;
	}

	// delete the materials
	if( pMaterials )
	{
		delete[] pMaterials;
		pMaterials = 0;
	}

	if( pMaterialIndexList )
	{
		delete[] pMaterialIndexList;
		pMaterialIndexList = 0;
	}

	nNumMaterialTriangleLists = 0;

	// delete tri per face numbers
	if( pTriPerMaterialList )
	{
		delete[] pTriPerMaterialList;
		pTriPerMaterialList = 0;
	}

	bValidLoad = false;

	return(0);
}

#endif // BASE_SUPPORT_OPENGL_GLSL
