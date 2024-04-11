
/*===================================================================
	File: Model.cpp
	Library: ModelLoaderLib

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"

#include "Model/ModelCommon.h"
#include "Model/Model.h"

#if defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
	#include "Model/FF/Mesh.h"
	#include "Model/FF/SkinMesh.h"
#elif defined(BASE_SUPPORT_OPENGL_GLSL)
	#include "Model/GLSL/MeshGLSL.h"
	#include "Model/GLSL/SkinMeshGLSL.h"
#endif 

using mdl::BoneAnimation;
using mdl::Model;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
BoneAnimation::BoneAnimation()
{
	std::memset( &AnimationFile, 0, sizeof( file::TFile ) );
	nAnimId = 99999;
	fAnimationFramerate = 0.0f;
	dTotalAnimationTime = 0.0;
	dCurrentTime = 0.0;
	fEndTime = 0.0f;
	fBlendWeight = 1.0f;
	nTotalJoints = 0;
	nTotalFrames = 0;
	bLooping = false;
	nCurrentFrame = 0;
	pFrames = 0;
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
BoneAnimation::~BoneAnimation()
{
	unsigned int i = 0;
	fAnimationFramerate = 0.0f;
	dTotalAnimationTime = 0.0;

	// delete the anims
	for( i = 0; i < nTotalJoints; i++ ) 
	{
		if( pFrames[i] )
		{
			delete[] pFrames[i];
			pFrames[i] = 0;
		}
	}

	if( pFrames )
	{
		delete[] pFrames;
		pFrames = 0;
	}

	dCurrentTime = 0.0;
	fEndTime = 0.0f;
	fBlendWeight = 1.0f;
	nTotalJoints = 0;
	nTotalFrames = 0;
	bLooping = false;
	nCurrentFrame = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// Method: SetMaterialData
/// Params: [in]nTexUnit, [in]pMaterial, [in]bInDisplayList
///
/////////////////////////////////////////////////////
void mdl::SetMaterialData( int nTexUnit, TMaterialEx *pMaterial )
{
	if( pMaterial->texUnit[nTexUnit].nTextureID != renderer::INVALID_OBJECT )
	{
		renderer::OpenGL::GetInstance()->BindUnitTexture( nTexUnit, GL_TEXTURE_2D, pMaterial->texUnit[nTexUnit].nTextureID );

#ifndef BASE_SUPPORT_OPENGL_GLSL
		if( pMaterial->texUnit[nTexUnit].fTextureScaleX != 1.0f ||
			pMaterial->texUnit[nTexUnit].fTextureScaleZ != 1.0f )
		{
			glMatrixMode(GL_TEXTURE);
				glLoadIdentity();
				glScalef( pMaterial->texUnit[nTexUnit].fTextureScaleX, pMaterial->texUnit[nTexUnit].fTextureScaleZ, 1.0f );
			glMatrixMode(GL_MODELVIEW);
		}
#endif // BASE_SUPPORT_OPENGL_GLSL
	}
	else
	{
		renderer::OpenGL::GetInstance()->DisableUnitTexture( nTexUnit );
	}
}

/////////////////////////////////////////////////////
/// Method: ResetMaterialData
/// Params: [in]nTexUnit, [in]pMaterial, [in]bInDisplayList
///
/////////////////////////////////////////////////////
void mdl::ResetMaterialData( int nTexUnit, TMaterialEx *pMaterial )
{
#ifndef BASE_SUPPORT_OPENGL_GLSL
	glActiveTexture( GL_TEXTURE0 + nTexUnit );

	if( pMaterial->texUnit[nTexUnit].fTextureScaleX != 1.0f ||
		pMaterial->texUnit[nTexUnit].fTextureScaleZ != 1.0f )
	{
		glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glScalef( 1.0f, 1.0f, 1.0f );
		glMatrixMode(GL_MODELVIEW);
	}	
	renderer::OpenGL::GetInstance()->ClearUnitTexture( nTexUnit );
#endif // BASE_SUPPORT_OPENGL_GLSL
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
Model::Model()
{
	Init();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
Model::~Model()
{
	Shutdown();
}

/////////////////////////////////////////////////////
/// Method: Init
/// Params: None
///
/////////////////////////////////////////////////////
void Model::Init( void )
{
	nGeneralFlags				= MODELFLAG_GENERAL_DRAWMODEL;

	nActiveProgram				= renderer::INVALID_OBJECT;

	nVertexBufferObject			= renderer::INVALID_OBJECT;
	nTextureBufferObject2D		= renderer::INVALID_OBJECT;
	nNormalBufferObject			= renderer::INVALID_OBJECT;

	bVBOInitialised						= false;

	bHasVertices		= false;
	bHasTextureCoords	= false;
	bHasNormals			= false;

	bSortMeshes			= false;

	nNumPoints			= 0;
	nNumVertices		= 0;
	nNumVertexColours	= 0;
	nNumNormals			= 0;
	nNumMaterials		= 0;
	nNumTextures		= 0;
	nNumTextureCoords	= 0;
	nNumBoneJoints		= 0;
	nNumBoneAnimations	= 0;
	nNumSkins			= 0;
	nNumOpaqueMeshes	= 0;
	nNumSortedMeshes	= 0;

	pPoints				= 0;
	pVertices			= 0;
	pNormals			= 0;
	pTexCoords			= 0;

	pMaterials			= 0;

	bHasAnimation		= false;
	pBoneJoints			= 0;
	BoneAnimationList.clear();

	pSkinMeshList		= 0;
	pOpaqueMeshList		= 0;
	pSortedMeshList		= 0;

	nFileSize = 0;

	vPos = math::Vec3( 0.0f, 0.0f, 0.0f );
	vRot = math::Vec3( 0.0f, 0.0f, 0.0f );
	qOri = math::Quaternion( 0.0f, 0.0f, 0.0f, 1.0f );

	std::memset( &ModelFile, 0, sizeof( file::TFile ) );

	bUseSubMeshFog = false;

	bDraw = true;
	bValidLoad = false;
}

/////////////////////////////////////////////////////
/// Method: Init
/// Params: None
///
/////////////////////////////////////////////////////
void Model::Initialise( void )
{
	Init();
}

/////////////////////////////////////////////////////
/// Method: DrawBoundingBox
/// Params: None
///
/////////////////////////////////////////////////////
void Model::DrawBoundingBox( void )
{
	if( !bValidLoad  )
		return;

	unsigned int i = 0;

	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		if( pOpaqueMeshList && pOpaqueMeshList[i].bDraw )
		{
			pOpaqueMeshList[i].DrawBoundingBox();
		}
	}
}

/////////////////////////////////////////////////////
/// Method: DrawBoundingSphere
/// Params: None
///
/////////////////////////////////////////////////////
void Model::DrawBoundingSphere( void )
{

}

/////////////////////////////////////////////////////
/// Method: SetModelGeneralFlags
/// Params: [in]newflags, [in]bToggle
///
/////////////////////////////////////////////////////
void Model::SetModelGeneralFlags( unsigned int newFlags )
{
	nGeneralFlags = newFlags;

	unsigned int i = 0;
	for( i = 0; i < nNumSkins; i++ )
	{
		if( pSkinMeshList )
		{
			pSkinMeshList[i].nGeneralFlags = nGeneralFlags;
		}
	}
	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		if( pOpaqueMeshList )
		{
			pOpaqueMeshList[i].nGeneralFlags = nGeneralFlags;
		}
	}
	for( i = 0; i < nNumSortedMeshes; i++ )
	{
		if( pSortedMeshList )
		{
			pSortedMeshList[i].nGeneralFlags = nGeneralFlags;
		}
	}
}	

/////////////////////////////////////////////////////
/// Method: SetModelDrawFlags
/// Params: [in]newflags, [in]bToggle
///
/////////////////////////////////////////////////////
void Model::SetModelDrawFlags( unsigned int newFlags, bool bToggle )
{
	if( bToggle )
		nDrawFlags ^= newFlags;
	else
		nDrawFlags |= newFlags;
}

/////////////////////////////////////////////////////
/// Method: SetActiveProgram
/// Params: [in]programId
///
/////////////////////////////////////////////////////
void Model::SetActiveProgram( GLuint programId )
{
#ifdef BASE_SUPPORT_OPENGL_GLSL
	//if( nActiveProgram != programId )
	{
		unsigned int i = 0;
		for( i = 0; i < nNumSkins; i++ )
		{
			if( pSkinMeshList )
			{
				pSkinMeshList[i].SetActiveProgram(programId);
			}
		}
		for( i = 0; i < nNumOpaqueMeshes; i++ )
		{
			if( pOpaqueMeshList )
			{
				pOpaqueMeshList[i].SetActiveProgram(programId);
			}
		}
		for( i = 0; i < nNumSortedMeshes; i++ )
		{
			if( pSortedMeshList )
			{
				pSortedMeshList[i].SetActiveProgram(programId);
			}
		}

		nActiveProgram = programId;
	}
#endif // BASE_SUPPORT_OPENGL_GLSL
}

/////////////////////////////////////////////////////
/// Method: Shutdown
/// Params: None
///
/////////////////////////////////////////////////////
int Model::Shutdown( void )
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

	// delete points
	if( pPoints )
	{
		delete[] pPoints;
		pPoints	= 0;
	}

	// delete vertices
	if( pVertices )
	{
		delete[] pVertices;
		pVertices	= 0;
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

	// delete the materials
	if( pMaterials )
	{
		delete[] pMaterials;
		pMaterials = 0;
	}

	// delete bone joints
	if( pBoneJoints )
	{
		delete[] pBoneJoints;
		pBoneJoints = 0;
	}

	// delete bone animations
	for( i=0; i < BoneAnimationList.size(); ++i )
	{
		delete BoneAnimationList[i];
		BoneAnimationList[i] = 0;
	}

	BoneAnimationList.clear();

	// delete any skinmeshes
	if( pSkinMeshList )
	{
		delete[] pSkinMeshList;
		pSkinMeshList = 0;
	}

	// delete any meshes
	if( pOpaqueMeshList )
	{
		delete[] pOpaqueMeshList;
		pOpaqueMeshList = 0;
	}
	if( pSortedMeshList )
	{
		delete[] pSortedMeshList;
		pSortedMeshList = 0;
	}

	bValidLoad = false;

	Init();

	return(0);
}


