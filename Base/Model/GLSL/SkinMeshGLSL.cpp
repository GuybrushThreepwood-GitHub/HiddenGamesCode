
/*===================================================================
	File: SkinMeshGLSL.cpp
	Library: ModelLoaderLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_OPENGL_GLSL

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"

#include "Model/ModelCommon.h"
#include "Model/Model.h"
#include "Model/GLSL/MeshGLSL.h"

#include "Model/GLSL/SkinMeshGLSL.h"

using mdl::SkinMesh;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
SkinMesh::SkinMesh()
{
	Initialise();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
SkinMesh::~SkinMesh()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void SkinMesh::Initialise( void )
{
	int i=0;

	nGeneralFlags				= 0;
	nDataFlushFlags				= 0;

	nVertexBufferObject			= renderer::INVALID_OBJECT;
	nTextureBufferObject2D		= renderer::INVALID_OBJECT;
	nNormalBufferObject			= renderer::INVALID_OBJECT;
	nTangentBufferObject		= renderer::INVALID_OBJECT;
	nElementBufferObject		= renderer::INVALID_OBJECT;

	nActiveProgram				= renderer::INVALID_OBJECT;
	nVertexAttribLocation		= -1;
	nNormalAttribLocation		= -1;
	nTexCoordsAttribLocation	= -1;
	nTangentAttribLocation		= -1;


	for( i=0; i < MODEL_MAX_USEABLE_TEXUNITS; ++i )
		nTexSamplerUniform[i]	= -1;

	bVBOInitialised						= false;

	bHasVertices		= true;
	bHasTextureCoords	= true;
	bHasNormals			= true;
	bHasTangents		= false;

	nNumVertexColours	= 0;
	nNumVertices		= 0;
	nNumPoints			= 0;
	nNumNormals			= 0;
	nNumTextureCoords	= 0;
	nNumMaterials		= 0;

	pPoints					= 0;
	pVertices				= 0;
	pFullVertexList			= 0;
	ppVertexPositionPointer = 0;
	pJointIndexList			= 0;
	pWeightList				= 0;
	pVertexWeights			= 0;
	pPerPointNormals		= 0;
	pNormals				= 0;
	pTangents				= 0;
	pFullNormalList			= 0;
	ppNormalPositionPointer = 0;
	pTexCoords				= 0;
	pMaterials				= 0;

	nNumMaterialTriangleLists = 0;
	pMaterialIndexList	= 0;
	pTriPerMaterialList	= 0;

	nTotalPolyCount		= 0;
	nTotalVertexCount	= 0;
	nTotalTriangleCount	= 0;

	nMeshId				= 99999;

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

	meshAABB.Reset();
	meshSphere.vCenterPoint = math::Vec3( 0.0f, 0.0f, 0.0f );
	meshSphere.fRadius = 1.0f;

	bIsSelected			= false;

	bDraw = false;
	bValidLoad = false;
}

/////////////////////////////////////////////////////
/// Method: SetActiveProgram
/// Params: [in]programId
///
/////////////////////////////////////////////////////
void SkinMesh::SetActiveProgram( GLuint programId )
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
void SkinMesh::Draw( void )
{
	if( core::app::GetUseVertexArrays() )
		DrawVA();
	else
		DrawVertexBufferObject();
}

/////////////////////////////////////////////////////
/// Method: SetupVertexBufferObject
/// Params: None
///
/////////////////////////////////////////////////////
void SkinMesh::SetupVertexBufferObject( void )
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

		if( bHasVertices &&
			nVertexAttribLocation != -1)
		{
			if( nVertexBufferObject == renderer::INVALID_OBJECT )
				glGenBuffers( 1, &nVertexBufferObject );						

			glBindBuffer( GL_ARRAY_BUFFER, nVertexBufferObject );	
			glBufferData( GL_ARRAY_BUFFER, nTotalVertexCount*sizeof(math::Vec3), pFullVertexList, GL_DYNAMIC_DRAW );
			
			glVertexAttribPointer( nVertexAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(math::Vec3), pFullVertexList );

			GL_CHECK			
		}

		if( bHasTextureCoords && 
			nTexCoordsAttribLocation != -1 )
		{
			//  default diffuse
			if( nTextureBufferObject2D == renderer::INVALID_OBJECT )
				glGenBuffers( 1, &nTextureBufferObject2D );		

			glBindBuffer( GL_ARRAY_BUFFER, nTextureBufferObject2D );	

			glBufferData( GL_ARRAY_BUFFER, nTotalVertexCount*sizeof(math::Vec2), pTexCoords, GL_STATIC_DRAW );

			glVertexAttribPointer( nTexCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), pTexCoords );			
	
			GL_CHECK		
		}

		if( bHasNormals &&
			nNormalAttribLocation != -1 )
		{
			if( nNormalBufferObject == renderer::INVALID_OBJECT )
				glGenBuffers( 1, &nNormalBufferObject );	

			glBindBuffer( GL_ARRAY_BUFFER, nNormalBufferObject );

			glBufferData( GL_ARRAY_BUFFER, nTotalVertexCount*sizeof(math::Vec3), pFullNormalList, GL_DYNAMIC_DRAW );

			glVertexAttribPointer( nNormalAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(math::Vec3), pFullNormalList );	
			
			GL_CHECK			
		}
		
		if( nTangentAttribLocation != -1 )
		{
			if( nTangentBufferObject == renderer::INVALID_OBJECT )
				glGenBuffers( 1, &nTangentBufferObject );		

			glBindBuffer( GL_ARRAY_BUFFER, nTangentBufferObject );

			glBufferData( GL_ARRAY_BUFFER, nTotalVertexCount*sizeof(math::Vec4), pTangents, GL_STATIC_DRAW );

			glVertexAttribPointer( nTangentAttribLocation, 4, GL_FLOAT, GL_FALSE, sizeof(math::Vec4), pTangents );

			GL_CHECK
		}

		bVBOInitialised = true;
	}

	// FIXME: these can be deleted if the shader doesn't change
	/*if( pTangents )
	{
		delete[] pTangents;
		pTangents = 0;
	}

	// delete texcoords
	if( pTexCoords )
	{
		delete[] pTexCoords;
		pTexCoords = 0;
	}*/

	//////////////////////////////////////

	GL_CHECK

	renderer::OpenGL::GetInstance()->UseProgram(prevProg);
	
	renderer::OpenGL::GetInstance()->DisableVBO();
}

/////////////////////////////////////////////////////
/// Method: DrawVertexBufferObject
/// Params: None
///
/////////////////////////////////////////////////////
void SkinMesh::DrawVertexBufferObject( void )
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
		math::Vec3 *pVertMappedBuffer = 0;
		math::Vec3 *pNormMappedBuffer = 0;

		if( bHasVertices &&
			nVertexAttribLocation != -1 )
		{
			if( nVertexBufferObject != renderer::INVALID_OBJECT )
			{
				// get the map for the verts
				if( GLEW_OES_mapbuffer )
				{
					glBindBuffer( GL_ARRAY_BUFFER, nVertexBufferObject );
					pVertMappedBuffer = (math::Vec3 *)glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );

					// get the map for the normals
					if( bHasNormals &&
						nNormalBufferObject != renderer::INVALID_OBJECT )
					{
						glBindBuffer( GL_ARRAY_BUFFER, nNormalBufferObject );
						pNormMappedBuffer = (math::Vec3 *)glMapBuffer( GL_ARRAY_BUFFER, GL_WRITE_ONLY );
					}
				}

				// switch back to verts
				glBindBuffer( GL_ARRAY_BUFFER, nVertexBufferObject );

				if( pVertMappedBuffer )
				{
					for( i = 0; i < nNumVertices; i++ )
					{
						// updates verts
						pVertMappedBuffer[i] = *ppVertexPositionPointer[i];

						// updates normals
						if( pNormMappedBuffer )
							pNormMappedBuffer[i] = *ppNormalPositionPointer[i]; 
					}
				}
				else
				{
					// mapped buffer not supported

					for( i = 0; i < nNumVertices; i++ )
					{
						// updates verts
						pFullVertexList[i] = *ppVertexPositionPointer[i];

						// updates normals
						if( bHasNormals )
							pFullNormalList[i] = *ppNormalPositionPointer[i]; 
					}

					glBufferData( GL_ARRAY_BUFFER, nTotalVertexCount*sizeof(math::Vec3), pFullVertexList, GL_DYNAMIC_DRAW );
				}
									
				glEnableVertexAttribArray( nVertexAttribLocation );
				glVertexAttribPointer( nVertexAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(math::Vec3), VBO_OFFSET(0) );
					
				if( pVertMappedBuffer )
				{
					// unmap vert buffer
					glUnmapBuffer( GL_ARRAY_BUFFER );
					pVertMappedBuffer = 0;
				}

				GL_CHECK				
			}
		}

		if( bHasTextureCoords &&
			nTexCoordsAttribLocation != -1)
		{
			if( nTextureBufferObject2D != renderer::INVALID_OBJECT )
			{
				glBindBuffer( GL_ARRAY_BUFFER, nTextureBufferObject2D );

				// optional
				for( i = 0; i < nNumMaterialTriangleLists; i++ )
				{
					// only if enabled
					for( j = MODEL_MAX_USEABLE_TEXUNITS-1; j >= 0; j-- )
					{
						if( pMaterials[i].texUnit[j].bEnabled  )// && j < MODEL_MAX_USEABLE_TEXUNITS
						{
							glEnableVertexAttribArray( nTexCoordsAttribLocation );
							glVertexAttribPointer( nTexCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), VBO_OFFSET(0) );
						}
					}
				}
			}
			
			GL_CHECK		
		}


		if( bHasNormals &&
			nNormalAttribLocation != -1 )
		{
			if( nNormalBufferObject != renderer::INVALID_OBJECT )
			{
				glBindBuffer( GL_ARRAY_BUFFER, nNormalBufferObject );

				// mapped buffer not supported
				if( !pNormMappedBuffer )
					glBufferData( GL_ARRAY_BUFFER, nTotalVertexCount*sizeof(math::Vec3), pFullNormalList, GL_DYNAMIC_DRAW );

				glEnableVertexAttribArray( nNormalAttribLocation );
				glVertexAttribPointer( nNormalAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(math::Vec3), VBO_OFFSET(0) );

				if( pNormMappedBuffer )
				{
					glUnmapBuffer( GL_ARRAY_BUFFER );
					pNormMappedBuffer = 0;
				}
			}			
			
			GL_CHECK			
		}

		if( nTangentAttribLocation != -1 )
		{
			if( nTangentBufferObject != renderer::INVALID_OBJECT )
			{
				glBindBuffer( GL_ARRAY_BUFFER, nTangentBufferObject );

				glEnableVertexAttribArray( nTangentAttribLocation );
				glVertexAttribPointer( nTangentAttribLocation, 4, GL_FLOAT, GL_FALSE, sizeof(math::Vec4), VBO_OFFSET(0) );
			}
		
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
/// Method: DrawVA
/// Params: None
///
/////////////////////////////////////////////////////
void SkinMesh::DrawVA( void )
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
			nVertexAttribLocation != -1)
		{
			for( i = 0; i < nNumVertices; i++ )
			{
				pFullVertexList[i] = *ppVertexPositionPointer[i]; 

				if( bHasNormals )
					pFullNormalList[i] = *ppNormalPositionPointer[i]; 
			}

			glEnableVertexAttribArray( nVertexAttribLocation );
			glVertexAttribPointer( nVertexAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(math::Vec3), pFullVertexList );
			
			GL_CHECK			
		}

		if( bHasTextureCoords && 
			nTexCoordsAttribLocation != -1 )
		{	
			glEnableVertexAttribArray( nTexCoordsAttribLocation );
			glVertexAttribPointer( nTexCoordsAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(math::Vec2), pTexCoords );
			
			GL_CHECK		
		}

		if( bHasNormals &&
			nNormalAttribLocation != -1 )
		{	
			glEnableVertexAttribArray( nNormalAttribLocation );
			glVertexAttribPointer( nNormalAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(math::Vec3), pFullNormalList );
			
			GL_CHECK			
		}

		if( nTangentAttribLocation != -1 )
		{
			glEnableVertexAttribArray( nTangentAttribLocation );
			glVertexAttribPointer( nTangentAttribLocation, 4, GL_FLOAT, GL_FALSE, sizeof(math::Vec4), pTangents );
		
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
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void SkinMesh::Release( void )
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
int SkinMesh::Shutdown( void )
{
	unsigned int i = 0;

	// delete buffer objects
	if( nVertexBufferObject != renderer::INVALID_OBJECT )
	{
		glDeleteBuffers( 1, &nVertexBufferObject );
		nVertexBufferObject = renderer::INVALID_OBJECT;
	}

	if( nTextureBufferObject2D != renderer::INVALID_OBJECT )
	{
		glDeleteBuffers( 1, &nTextureBufferObject2D );
		nTextureBufferObject2D = renderer::INVALID_OBJECT;
	}
	if( nNormalBufferObject != renderer::INVALID_OBJECT )
	{
		glDeleteBuffers( 1, &nNormalBufferObject );
		nNormalBufferObject = renderer::INVALID_OBJECT;
	}
	if( nTangentBufferObject != renderer::INVALID_OBJECT )
	{
		glDeleteBuffers( 1, &nTangentBufferObject );
		nTangentBufferObject = renderer::INVALID_OBJECT;
	}
	if( nElementBufferObject != renderer::INVALID_OBJECT )
	{
		glDeleteBuffers( 1, &nElementBufferObject );
		nElementBufferObject = renderer::INVALID_OBJECT;
	}

	// delete points
	if( pPoints )
	{
		delete[] pPoints;
		pPoints	= 0;
	}

	if( pFullVertexList )
	{
		delete[] pFullVertexList;
		pFullVertexList = 0;
	}

	// delete pointer to vertices
	if( ppVertexPositionPointer )
	{
		delete[] ppVertexPositionPointer;
		ppVertexPositionPointer = 0;
	}

	// delete vertices
	if( pVertices )
	{
		delete[] pVertices;
		pVertices	= 0;
	}

	// delete joint index list
	if( pJointIndexList )
	{
		delete[] pJointIndexList;
		pJointIndexList	= 0;
	}

	// delete vertex weight list
	if( pWeightList )
	{
		delete[] pWeightList;
		pWeightList	= 0;
	}

	// delete vertex weight list
	if( pVertexWeights )
	{
		for( i = 0; i < nNumPoints; i++ )
		{
			delete[] pVertexWeights[i];
		}

		delete[] pVertexWeights;
		pVertexWeights	= 0;
	}

	// delete normals
	if( pPerPointNormals )
	{
		delete[] pPerPointNormals;
		pPerPointNormals = 0;
	}

	if( pNormals )
	{
		delete[] pNormals;
		pNormals = 0;
	}

	if( pTangents )
	{
		delete[] pTangents;
		pTangents = 0;
	}

	if( pFullNormalList )
	{
		delete[] pFullNormalList;
		pFullNormalList = 0;
	}

	// delete pointer to vertices
	if( ppNormalPositionPointer )
	{
		delete[] ppNormalPositionPointer;
		ppNormalPositionPointer = 0;
	}

	// delete texcoords
	if( pTexCoords )
	{
		delete[] pTexCoords;
		pTexCoords = 0;
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

	// delete tri per face numbers
	if( pTriPerMaterialList )
	{
		delete[] pTriPerMaterialList;
		pTriPerMaterialList = 0;
	}

	return(0);
}

#endif // BASE_SUPPORT_OPENGL_GLSL
