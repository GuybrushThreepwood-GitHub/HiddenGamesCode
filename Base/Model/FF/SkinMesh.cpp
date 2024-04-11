
/*===================================================================
	File: SkinMesh.cpp
	Library: ModelLoaderLib

	(C)Hidden Games
=====================================================================*/

#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"

#include "Model/ModelCommon.h"
#include "Model/Model.h"
#include "Model/FF/Mesh.h"

#include "Model/FF/SkinMesh.h"

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
	nGeneralFlags				= 0;
	nDataFlushFlags				= 0;

	nVertexBufferObject			= renderer::INVALID_OBJECT;
	nTextureBufferObject2D		= renderer::INVALID_OBJECT;
	nNormalBufferObject			= renderer::INVALID_OBJECT;
	nElementBufferObject		= renderer::INVALID_OBJECT;

	bVBOInitialised						= false;

	bHasVertices		= true;
	bHasTextureCoords	= true;
	bHasNormals			= true;

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
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void SkinMesh::Draw( void )
{
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
void SkinMesh::SetupVertexBufferObject( void )
{
	if( core::app::GetUseVertexArrays() ||
		!renderer::bExtVertexBufferObjects)
		return;

	if( !bVBOInitialised )
	{
		if( bHasVertices )
		{
			glGenBuffers( 1, &nVertexBufferObject );						
			glBindBuffer( GL_ARRAY_BUFFER, nVertexBufferObject );	
			glBufferData( GL_ARRAY_BUFFER, nTotalVertexCount*sizeof(math::Vec3), pFullVertexList, GL_DYNAMIC_DRAW );
			glVertexPointer( 3, GL_FLOAT, sizeof(math::Vec3), pFullVertexList );
			
			GL_CHECK			
		}

		if( bHasTextureCoords )
		{
			glClientActiveTexture( GL_TEXTURE0 );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			
			//  default diffuse
			glGenBuffers( 1, &nTextureBufferObject2D );						
			glBindBuffer( GL_ARRAY_BUFFER, nTextureBufferObject2D );	

			glBufferData( GL_ARRAY_BUFFER, nTotalVertexCount*sizeof(math::Vec2), pTexCoords, GL_STATIC_DRAW );
			glTexCoordPointer( 2, GL_FLOAT, sizeof( math::Vec2 ), pTexCoords );
			
			GL_CHECK		
		}

		if( bHasNormals )
		{
			renderer::OpenGL::GetInstance()->EnableNormalArray();
			
			glGenBuffers( 1, &nNormalBufferObject );						
			glBindBuffer( GL_ARRAY_BUFFER, nNormalBufferObject );

			glBufferData( GL_ARRAY_BUFFER, nTotalVertexCount*sizeof(math::Vec3), pFullNormalList, GL_DYNAMIC_DRAW );
			glNormalPointer( GL_FLOAT, sizeof( math::Vec3 ), pFullNormalList );
			
			GL_CHECK			
		}
		
		bVBOInitialised = true;
	}
	
	// delete normals
	if( pNormals )
	{
		delete[] pNormals;
		pNormals = 0;
	}

	// delete texcoords
	if( pTexCoords )
	{
		delete[] pTexCoords;
		pTexCoords = 0;
	}

	//////////////////////////////////////
	
	glClientActiveTexture( GL_TEXTURE0 );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	if( bHasNormals )
		renderer::OpenGL::GetInstance()->DisableNormalArray();
		
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

		if( bHasVertices )
		{
			if( nVertexBufferObject != renderer::INVALID_OBJECT )
			{
				// get the map for the verts
				if( renderer::bExtMappedBuffer )
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
				
				glVertexPointer( 3, GL_FLOAT, 0, (char *) 0 );
				
				if( pVertMappedBuffer )
				{
					glUnmapBuffer( GL_ARRAY_BUFFER );
					pVertMappedBuffer = 0;
				}

				GL_CHECK				
			}
		}

		if( bHasTextureCoords )
		{
			if( nTextureBufferObject2D != renderer::INVALID_OBJECT )
			{
				// use the same uv coords if coords are not generated
				// optional
				for( i = 0; i < nNumMaterialTriangleLists; i++ )
				{
					for( j = MODEL_MAX_USEABLE_TEXUNITS-1; j >= 0; j-- )
					{
						// only if enabled
						if( pMaterials[i].texUnit[j].bEnabled )
						{
							glClientActiveTexture( GL_TEXTURE0+j );

							glEnableClientState( GL_TEXTURE_COORD_ARRAY );
							if( j != 0 )
							{
								if( renderer::OpenGL::GetInstance()->GetTextureState() )
									glEnable(GL_TEXTURE_2D);
							}

								glBindBuffer( GL_ARRAY_BUFFER, nTextureBufferObject2D );

								glTexCoordPointer( 2, GL_FLOAT, 0, (char *) 0 );
						}
					}
				}
			}
			
			GL_CHECK		
		}

		if( bHasNormals )
		{
			if( nNormalBufferObject != renderer::INVALID_OBJECT )
			{
				glBindBuffer( GL_ARRAY_BUFFER, nNormalBufferObject );

				// mapped buffer not supported
				if( !pNormMappedBuffer )
					glBufferData( GL_ARRAY_BUFFER, nTotalVertexCount*sizeof(math::Vec3), pFullNormalList, GL_DYNAMIC_DRAW );
				
				glNormalPointer( GL_FLOAT, 0, (char *) 0 );

				if( pNormMappedBuffer )
				{
					glUnmapBuffer( GL_ARRAY_BUFFER );
					pNormMappedBuffer = 0;
				}
			}			
			
			GL_CHECK			
		}

		for( i = 0; i < nNumMaterialTriangleLists; i++ )
		{
			endIndices = ( (3*pTriPerMaterialList[i]) )+startIndices;

			renderer::OpenGL::GetInstance()->DisableColourMaterial();
			renderer::OpenGL::GetInstance()->SetMaterialAmbient( pMaterials[i].Ambient );
			renderer::OpenGL::GetInstance()->SetMaterialDiffuse( pMaterials[i].Diffuse );

			renderer::OpenGL::GetInstance()->SetMaterialSpecular( pMaterials[i].Specular );
			renderer::OpenGL::GetInstance()->SetMaterialEmission( pMaterials[i].Emissive );
			renderer::OpenGL::GetInstance()->SetMaterialShininess( pMaterials[i].Shininess );

			for( j = MODEL_MAX_USEABLE_TEXUNITS-1; j >= 0; j-- )
			{
				// only if enabled
				if( pMaterials[i].texUnit[j].bEnabled )
				{
					SetMaterialData( j, &pMaterials[i] );
				}
				//else
				//	renderer::OpenGL::GetInstance()->DisableUnitTexture( j );	
			}

			glDrawArrays(GL_TRIANGLES, startIndices, (3*pTriPerMaterialList[i]));

			for( j = MODEL_MAX_USEABLE_TEXUNITS-1; j >= 0; j-- )
			{
				if( pMaterials[i].texUnit[j].bEnabled )
				{
					ResetMaterialData( j, &pMaterials[i] );
				}
			}

			startIndices = endIndices;

			GL_CHECK
		}

		if( bHasTextureCoords )
		{
			for( i = 0; i < nNumMaterialTriangleLists; i++ )
			{
				for( j = MODEL_MAX_USEABLE_TEXUNITS-1; j >= 0; j-- )
				{
					// only if enabled
					if( pMaterials[i].texUnit[j].bEnabled )
					{
						glClientActiveTexture( GL_TEXTURE0+j );
						glDisableClientState( GL_TEXTURE_COORD_ARRAY );

						if( j != 0 )
						{
							if( renderer::OpenGL::GetInstance()->GetTextureState() )
								glDisable(GL_TEXTURE_2D);
						}

						//renderer::OpenGL::GetInstance()->DisableUnitTexture( j );
					}
				}
			}
		}
	}
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

	renderer::OpenGL::GetInstance()->DisableVBO();

	unsigned int i = 0;
	int j = 0;	
	int startIndices, endIndices;

	startIndices = endIndices = 0;
	if( nNumMaterialTriangleLists )
	{
		if( bHasVertices )
		{
			for( i = 0; i < nNumVertices; i++ )
			{
				pFullVertexList[i] = *ppVertexPositionPointer[i]; 
				pFullNormalList[i] = *ppNormalPositionPointer[i]; 
			}
			glVertexPointer( 3, GL_FLOAT, sizeof(math::Vec3), pFullVertexList );
			
			GL_CHECK			
		}

		if( bHasTextureCoords )
		{
			glClientActiveTexture( GL_TEXTURE0 );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			
			glTexCoordPointer( 2, GL_FLOAT, sizeof( math::Vec2 ), pTexCoords );
			
			GL_CHECK		
		}

		if( bHasNormals )
		{
			renderer::OpenGL::GetInstance()->EnableNormalArray();
			
			glNormalPointer( GL_FLOAT, sizeof(math::Vec3), pFullNormalList );

			GL_CHECK			
		}

		for( i = 0; i < nNumMaterialTriangleLists; i++ )
		{
			endIndices = ( (3*pTriPerMaterialList[i]) )+startIndices;

			renderer::OpenGL::GetInstance()->DisableColourMaterial();
			renderer::OpenGL::GetInstance()->SetMaterialAmbient( pMaterials[i].Ambient );
			renderer::OpenGL::GetInstance()->SetMaterialDiffuse( pMaterials[i].Diffuse );

			renderer::OpenGL::GetInstance()->SetMaterialSpecular( pMaterials[i].Specular );
			renderer::OpenGL::GetInstance()->SetMaterialEmission( pMaterials[i].Emissive );
			renderer::OpenGL::GetInstance()->SetMaterialShininess( pMaterials[i].Shininess );

			for( j = MODEL_MAX_USEABLE_TEXUNITS-1; j >= 0; j-- )
			{
				// only if enabled
				if( pMaterials[i].texUnit[j].bEnabled )
				{
					SetMaterialData( j, &pMaterials[i] );
				}
				//else
				//	renderer::OpenGL::GetInstance()->DisableUnitTexture( j );	
			}

			glDrawArrays( GL_TRIANGLES, startIndices, (3*pTriPerMaterialList[i]) );

			startIndices = endIndices;

			GL_CHECK
		}

		if( bHasTextureCoords )
		{
			for( i = 0; i < nNumMaterialTriangleLists; i++ )
			{
				for( j = MODEL_MAX_USEABLE_TEXUNITS-1; j >= 0; j-- )
				{
					// only if enabled
					//if( pMaterials[i].texUnit[j].bEnabled )
					{
						glClientActiveTexture( GL_TEXTURE0+j );
						glDisableClientState( GL_TEXTURE_COORD_ARRAY );
						if( j != 0 )
						{
							if( renderer::OpenGL::GetInstance()->GetTextureState() )
								glDisable(GL_TEXTURE_2D);
						}
						//renderer::OpenGL::GetInstance()->DisableUnitTexture( j );
					}
				}
			}
		}
	}
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
	int j = 0;

	// delete buffer objects
	if( nVertexBufferObject != renderer::INVALID_OBJECT )
	{
		glDeleteBuffers( 1, &nVertexBufferObject );
		nVertexBufferObject = renderer::INVALID_OBJECT;
	}

	if( nTextureBufferObject2D != renderer::INVALID_OBJECT )
	{
		for( j = MODEL_MAX_USEABLE_TEXUNITS-1; j >= 0; j-- )
		{
			// only if enabled
			//if( pMaterials[i].texUnit[j].bEnabled )
			{
				glClientActiveTexture( GL_TEXTURE0+j );
				glDisableClientState( GL_TEXTURE_COORD_ARRAY );

				if( j != 0 )
				{
					if( renderer::OpenGL::GetInstance()->GetTextureState() )
						glEnable(GL_TEXTURE_2D);
				}

				//renderer::OpenGL::GetInstance()->DisableUnitTexture( j );
			}
		}

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

	if( pPerPointNormals )
	{
		delete[] pPerPointNormals;
		pPerPointNormals = 0;
	}

	// delete normals
	if( pNormals )
	{
		delete[] pNormals;
		pNormals = 0;
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

#endif // defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
