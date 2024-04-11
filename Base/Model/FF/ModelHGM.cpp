
/*===================================================================
	File: ModelHGM.cpp
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

#include "Model/FF/ModelHGM.h"

using mdl::ModelHGM;

namespace
{
	math::Vec4Lite defaultWhite( 255,255,255,255 );
}

/////////////////////////////////////////////////////
int TextureSort( const void *pMesh1, const void *pMesh2 );

/////////////////////////////////////////////////////
/// Function: TextureSort
/// Params: [in]pMesh1, [in]mesh2
///
/////////////////////////////////////////////////////
int TextureSort( const void *pMesh1, const void *pMesh2 )
{
	mdl::Mesh *pFirst = (mdl::Mesh*) pMesh1;
	mdl::Mesh *pSecond = (mdl::Mesh*) pMesh2;
	
	if( pFirst->nMeshIndex == pSecond->nMeshIndex )
	{
		return( pFirst->pMaterials[0].texUnit[0].nTexLookup < pSecond->pMaterials[0].texUnit[0].nTexLookup) ? -1:1;
	}
	return (-1);
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
ModelHGM::ModelHGM()
{
	Initialise();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
ModelHGM::~ModelHGM()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void ModelHGM::Initialise( void )
{
	// Model initialise
	Init();

	m_TotalPolyCount		= 0;
	m_TotalTriCount		= 0;

	modelAABB.Reset();
	modelSphere.vCenterPoint = math::Vec3( 0.0f, 0.0f, 0.0f );
	modelSphere.fRadius = 1.0f;

	nGeneralFlags			= 0;//MODELFLAG_GENERAL_AUTOCULLING;

	m_TextureFormatList.clear();
	m_MaterialFormatList.clear();

	m_TextureIDList.clear();
	m_MaterialBlockList.clear();

	m_DefaultMeshColour = math::Vec4Lite( 255,255,255,255 );
	m_UseDefaultColour = false;

	m_NumAnimationBlocks = 0;
	m_TexAnimationBlocks = 0;
}

/////////////////////////////////////////////////////
/// Method: Load
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int ModelHGM::Load( const char *szFilename, GLenum magFilter, GLenum minFilter, bool lowPower )
{
	file::TFileHandle fileHandle;
	unsigned int i = 0, j = 0, k = 0;

	if( core::IsEmptyString( szFilename ) )
		return(1);

	if( file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, &fileHandle ) == false )
	{
		DBGLOG( "MODELHGM: *ERROR* Could not open %s file\n", szFilename );
		return(1);
	}

	// find length of file
	nFileSize = fileHandle.nFileLength;

	file::CreateFileStructure( szFilename, &ModelFile );

	file::FileRead( &m_TextureHeader, sizeof(mdl::TextureHeader), &fileHandle );

	m_TextureHeader.nTextureCount	= core::EndianSwapInt( m_TextureHeader.nTextureCount, core::MACHINE_LITTLE_ENDIAN );
	m_TextureHeader.nInternal		= core::EndianSwapInt( m_TextureHeader.nInternal, core::MACHINE_LITTLE_ENDIAN );
	m_TextureHeader.nFilesStart		= static_cast<unsigned int>(core::EndianSwapInt( static_cast<unsigned int>(m_TextureHeader.nFilesStart), core::MACHINE_LITTLE_ENDIAN ));
	m_TextureHeader.nFilesEnd		= static_cast<unsigned int>(core::EndianSwapInt( static_cast<unsigned int>(m_TextureHeader.nFilesEnd), core::MACHINE_LITTLE_ENDIAN ));
	m_TextureHeader.nTotalBlockSize	= static_cast<unsigned int>(core::EndianSwapInt( static_cast<unsigned int>(m_TextureHeader.nTotalBlockSize), core::MACHINE_LITTLE_ENDIAN ));

	GLuint texID = renderer::INVALID_OBJECT;
	mdl::TextureFormat textureType;

	nNumTextures = m_TextureHeader.nTextureCount;

	for( i=0; i < m_TextureHeader.nTextureCount; ++i )
	{
		file::FileRead( &textureType, sizeof(mdl::TextureFormat), &fileHandle );

		textureType.eTexFormat = static_cast<renderer::ETextureFormat>( core::EndianSwapInt( static_cast<unsigned int>(textureType.eTexFormat), core::MACHINE_LITTLE_ENDIAN ) );

		// add to the type list
		m_TextureFormatList.push_back(textureType);

		// missing ?
		if( textureType.eTexFormat != renderer::TEXTURE_MISSING_UNSUPPORTED )
		{
			bool genMipMaps = false;
			if( minFilter != GL_NEAREST &&
				minFilter != GL_LINEAR )
				genMipMaps = true;

			if( m_TextureHeader.nInternal )
				texID = renderer::TextureLoad( 0, m_TextureLoader, textureType.eTexFormat, &fileHandle, genMipMaps, magFilter, minFilter );
			else
			{
				file::TFile hgmPath;
				char texturePath[core::MAX_PATH+core::MAX_PATH];
				char texFile[core::MAX_PATH];
				std::memset( &texFile, 0, sizeof(char)*core::MAX_PATH );
				std::memset( &texturePath, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );

				file::FileReadTerminatedString( texFile, core::MAX_PATH-1, &fileHandle );
				
				// build the textures path
				file::CreateFileStructure( szFilename, &hgmPath );
				
				renderer::ETextureFormat texFormat = renderer::TEXTURE_UNKNOWN;
#if defined(BASE_PLATFORM_WINDOWS) || defined(BASE_PLATFORM_ANDROID) || defined(BASE_PLATFORM_RASPBERRYPI)|| defined(BASE_PLATFORM_MAC)
				// test tga
				snprintf( texturePath, core::MAX_PATH+core::MAX_PATH, "%s%stextures/%s.tga", hgmPath.szDrive, hgmPath.szPath, texFile );
				texFormat = renderer::TEXTURE_TGA;

				// test png
				if( !file::FileExists(texturePath) )
				{
					snprintf( texturePath, core::MAX_PATH+core::MAX_PATH, "%s%stextures/%s.png", hgmPath.szDrive, hgmPath.szPath, texFile );
					texFormat = renderer::TEXTURE_PNG;
				}
#else
				if( lowPower )
				{
					// test pvr
					snprintf( texturePath, core::MAX_PATH+core::MAX_PATH, "%s%stextures/%s.pvr", hgmPath.szDrive, hgmPath.szPath, texFile );
					texFormat = renderer::TEXTURE_PVR;

					// test png
					if( !file::FileExists(texturePath) )
					{
						snprintf( texturePath, core::MAX_PATH+core::MAX_PATH, "%s%stextures/%s.png", hgmPath.szDrive, hgmPath.szPath, texFile );
						texFormat = renderer::TEXTURE_PNG;
					}
				}
				else
				{
					snprintf( texturePath, core::MAX_PATH+core::MAX_PATH, "%s%stextures/%s.png", hgmPath.szDrive, hgmPath.szPath, texFile );
					texFormat = renderer::TEXTURE_PNG;
				}
#endif // BASE_PLATFORM_iOS

				texID = renderer::TextureLoad( texturePath, m_TextureLoader, texFormat, 0, genMipMaps, magFilter, minFilter );
			}

			// clear texture data
			m_TextureLoader.Free();
		}
		
		// add to generated ID list
		m_TextureIDList.push_back( texID );
	}


	file::FileRead( &m_MaterialHeader, sizeof(mdl::MaterialHeader), &fileHandle );

	m_MaterialHeader.nMaterialCount		= core::EndianSwapInt( m_MaterialHeader.nMaterialCount, core::MACHINE_LITTLE_ENDIAN );
	m_MaterialHeader.nMaterialsStart	= static_cast<unsigned int>(core::EndianSwapInt( static_cast<unsigned int>(m_MaterialHeader.nMaterialsStart), core::MACHINE_LITTLE_ENDIAN ));
	m_MaterialHeader.nMaterialsEnd		= static_cast<unsigned int>(core::EndianSwapInt( static_cast<unsigned int>(m_MaterialHeader.nMaterialsEnd), core::MACHINE_LITTLE_ENDIAN ));
	m_MaterialHeader.nTotalBlockSize	= static_cast<unsigned int>(core::EndianSwapInt( static_cast<unsigned int>(m_MaterialHeader.nTotalBlockSize), core::MACHINE_LITTLE_ENDIAN ));

	mdl::MaterialFormat matType;
	mdl::Material materialBlock;

	nNumMaterials = m_MaterialHeader.nMaterialCount;

	DBG_MEMTRY
		pMaterials = new TMaterialEx[nNumMaterials];
	DBG_MEMCATCH

	for( i=0; i < m_MaterialHeader.nMaterialCount; ++i )
	{
		pMaterials[i].nIndex = i;	
		std::memset( &pMaterials[i], 0, sizeof(TMaterialEx) );
		// do not zero out texture scale
		pMaterials[i].texUnit[0].fTextureScaleX = 1.0f;
		pMaterials[i].texUnit[0].fTextureScaleZ = 1.0f;

		// clear block
		std::memset( &materialBlock, 0, sizeof( mdl::Material ) );

		// read the type of material
		file::FileRead( &matType, sizeof(mdl::MaterialFormat), &fileHandle );

		matType.eMatFormat = static_cast<mdl::mat::EMaterialType>( core::EndianSwapInt( static_cast<unsigned int>(matType.eMatFormat), core::MACHINE_LITTLE_ENDIAN ) );

		// add to the type list
		m_MaterialFormatList.push_back(matType);

		// missing ?
		if( matType.eMatFormat != mdl::mat::MATTYPE_UNSUPPORTED )
		{
			if( matType.eMatFormat == mdl::mat::MATTYPE_LAMBERT )
			{
				// read a lambert block
				materialBlock.eMatFormat = mdl::mat::MATTYPE_LAMBERT;
				file::FileReadLambertBlock( &materialBlock.lambertBlock, sizeof(mdl::mat::LambertShaderBlock), 1, &fileHandle );

				pMaterials[i].Ambient = materialBlock.lambertBlock.ambientColour;
				//if( materialBlock.lambertBlock.colourIdx == -1 )
				//	pMaterials[i].Diffuse = math::Vec4( materialBlock.lambertBlock.colour.R*materialBlock.lambertBlock.diffuseCoeff, materialBlock.lambertBlock.colour.G*materialBlock.lambertBlock.diffuseCoeff, materialBlock.lambertBlock.colour.B*materialBlock.lambertBlock.diffuseCoeff, 1.0f );
				//else
					pMaterials[i].Diffuse = math::Vec4( materialBlock.lambertBlock.diffuseCoeff, materialBlock.lambertBlock.diffuseCoeff, materialBlock.lambertBlock.diffuseCoeff, 1.0f );
				pMaterials[i].Specular = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
				pMaterials[i].Emissive = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
				pMaterials[i].Shininess = 0.0f;

				if( materialBlock.lambertBlock.colourIdx != -1 )
				{
					pMaterials[i].texUnit[0].bEnabled = true;
					pMaterials[i].texUnit[0].nTexLookup = materialBlock.lambertBlock.colourIdx;
					if( pMaterials[i].texUnit[0].nTexLookup != -1 )
						pMaterials[i].texUnit[0].nTextureID = m_TextureIDList[pMaterials[i].texUnit[0].nTexLookup];
					else
						pMaterials[i].texUnit[0].nTextureID = renderer::INVALID_OBJECT;
				}

				if( materialBlock.lambertBlock.bump.hasBump )
				{
					pMaterials[i].texUnit[2].bEnabled = true;
					pMaterials[i].texUnit[2].nTexLookup = materialBlock.lambertBlock.bump.bumpIdx;
					if( pMaterials[i].texUnit[2].nTexLookup != -1 )
						pMaterials[i].texUnit[2].nTextureID = m_TextureIDList[pMaterials[i].texUnit[2].nTexLookup];
					else
						pMaterials[i].texUnit[2].nTextureID = renderer::INVALID_OBJECT;
				}

//#ifdef _DEBUG
//				DBGLOG( "Lambert Block:\n" );
//				DBGLOG( "\t|diffuseCoeff  %.2f\n", materialBlock.lambertBlock.diffuseCoeff );
//				DBGLOG( "\t|colour  (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.lambertBlock.colour.R, materialBlock.lambertBlock.colour.G, materialBlock.lambertBlock.colour.B, materialBlock.lambertBlock.colour.A );
//				DBGLOG( "\t|colourIdx  %d\n", materialBlock.lambertBlock.colourIdx );
//				DBGLOG( "\t|transparency  (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.lambertBlock.transparency.R, materialBlock.lambertBlock.transparency.G, materialBlock.lambertBlock.transparency.B, materialBlock.lambertBlock.transparency.A );
//				DBGLOG( "\t|transparencyIdx  %d\n", materialBlock.lambertBlock.transparencyIdx );
//				DBGLOG( "\t|ambientColour (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.lambertBlock.ambientColour.R, materialBlock.lambertBlock.ambientColour.G, materialBlock.lambertBlock.ambientColour.B, materialBlock.lambertBlock.ambientColour.A );
//				DBGLOG( "\t|ambientColourIdx  %d\n", materialBlock.lambertBlock.ambientColourIdx );
//				DBGLOG( "\t|incandescence  (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.lambertBlock.incandescence.R, materialBlock.lambertBlock.incandescence.G, materialBlock.lambertBlock.incandescence.B, materialBlock.lambertBlock.incandescence.A );
//				DBGLOG( "\t|incandescenceIdx  %d\n", materialBlock.lambertBlock.incandescenceIdx );
//				DBGLOG( "\t|translucenceCoeff  %.2f\n", materialBlock.lambertBlock.translucenceCoeff );
//				DBGLOG( "\t|glowIntensity  %.2f\n", materialBlock.lambertBlock.glowIntensity );
//				DBGLOG( "\t\t|hasBump  %d\n", materialBlock.lambertBlock.bump.hasBump );
//				DBGLOG( "\t\t|bumpDepth  %.2f\n", materialBlock.lambertBlock.bump.bumpDepth );
//				DBGLOG( "\t\t|bumpIdx  %d\n", materialBlock.lambertBlock.bump.bumpIdx );
//#endif // _DEBUG
			}
			else if( matType.eMatFormat == mdl::mat::MATTYPE_BLINN )
			{
				// read a blinn block
				materialBlock.eMatFormat = mdl::mat::MATTYPE_BLINN;
				file::FileReadBlinnBlock( &materialBlock.blinnBlock, sizeof(mdl::mat::BlinnShaderBlock), 1, &fileHandle );	

				pMaterials[i].Ambient = materialBlock.blinnBlock.lambert.ambientColour;
				//if( materialBlock.blinnBlock.lambert.colourIdx == -1 )
				//	pMaterials[i].Diffuse = math::Vec4( materialBlock.blinnBlock.lambert.colour.R*materialBlock.blinnBlock.lambert.diffuseCoeff, materialBlock.blinnBlock.lambert.colour.G*materialBlock.blinnBlock.lambert.diffuseCoeff, materialBlock.blinnBlock.lambert.colour.B*materialBlock.blinnBlock.lambert.diffuseCoeff, 1.0f );
				//else
					pMaterials[i].Diffuse = math::Vec4( materialBlock.blinnBlock.lambert.diffuseCoeff, materialBlock.blinnBlock.lambert.diffuseCoeff, materialBlock.blinnBlock.lambert.diffuseCoeff, 1.0f );
				if( materialBlock.blinnBlock.reflect.specularColourIdx == -1 )
					pMaterials[i].Specular = materialBlock.blinnBlock.reflect.specularColour;
				pMaterials[i].Emissive = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
				pMaterials[i].Shininess = 0.0f;

				if( materialBlock.blinnBlock.lambert.colourIdx != -1 )
				{
					pMaterials[i].texUnit[0].bEnabled = true;
					pMaterials[i].texUnit[0].nTexLookup = materialBlock.blinnBlock.lambert.colourIdx;
					if( pMaterials[i].texUnit[0].nTexLookup != -1 )
						pMaterials[i].texUnit[0].nTextureID = m_TextureIDList[pMaterials[i].texUnit[0].nTexLookup];
					else
						pMaterials[i].texUnit[0].nTextureID = renderer::INVALID_OBJECT;
				}

				if( materialBlock.blinnBlock.reflect.specularColourIdx != -1 )
				{
					pMaterials[i].texUnit[1].bEnabled = true;
					pMaterials[i].texUnit[1].nTexLookup = materialBlock.blinnBlock.reflect.specularColourIdx;
					if( pMaterials[i].texUnit[1].nTexLookup != -1 )
						pMaterials[i].texUnit[1].nTextureID = m_TextureIDList[pMaterials[i].texUnit[1].nTexLookup];
					else
						pMaterials[i].texUnit[1].nTextureID = renderer::INVALID_OBJECT;

					pMaterials[i].Specular = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
				}

				if( materialBlock.blinnBlock.lambert.bump.hasBump )
				{
					pMaterials[i].texUnit[2].bEnabled = true;
					pMaterials[i].texUnit[2].nTexLookup = materialBlock.blinnBlock.lambert.bump.bumpIdx;
					if( pMaterials[i].texUnit[2].nTexLookup != -1 )
						pMaterials[i].texUnit[2].nTextureID = m_TextureIDList[pMaterials[i].texUnit[2].nTexLookup];
					else
						pMaterials[i].texUnit[2].nTextureID = renderer::INVALID_OBJECT;
				}
//#ifdef _DEBUG
//				DBGLOG( "Blinn Block:\n" );
//				DBGLOG( "\t|diffuseCoeff  %.2f\n", materialBlock.blinnBlock.lambert.diffuseCoeff );
//				DBGLOG( "\t|colour  (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.blinnBlock.lambert.colour.R, materialBlock.blinnBlock.lambert.colour.G, materialBlock.blinnBlock.lambert.colour.B, materialBlock.blinnBlock.lambert.colour.A );
//				DBGLOG( "\t|colourIdx  %d\n", materialBlock.blinnBlock.lambert.colourIdx );
//				DBGLOG( "\t|transparency  (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.blinnBlock.lambert.transparency.R, materialBlock.blinnBlock.lambert.transparency.G, materialBlock.blinnBlock.lambert.transparency.B, materialBlock.blinnBlock.lambert.transparency.A );
//				DBGLOG( "\t|transparencyIdx  %d\n", materialBlock.blinnBlock.lambert.transparencyIdx );
//				DBGLOG( "\t|ambientColour (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.blinnBlock.lambert.ambientColour.R, materialBlock.blinnBlock.lambert.ambientColour.G, materialBlock.blinnBlock.lambert.ambientColour.B, materialBlock.blinnBlock.lambert.ambientColour.A );
//				DBGLOG( "\t|ambientColourIdx  %d\n", materialBlock.blinnBlock.lambert.ambientColourIdx );
//				DBGLOG( "\t|incandescence  (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.blinnBlock.lambert.incandescence.R, materialBlock.blinnBlock.lambert.incandescence.G, materialBlock.blinnBlock.lambert.incandescence.B, materialBlock.blinnBlock.lambert.incandescence.A );
//				DBGLOG( "\t|incandescenceIdx  %d\n", materialBlock.blinnBlock.lambert.incandescenceIdx );
//				DBGLOG( "\t|translucenceCoeff  %.2f\n", materialBlock.blinnBlock.lambert.translucenceCoeff );
//				DBGLOG( "\t|glowIntensity  %.2f\n", materialBlock.blinnBlock.lambert.glowIntensity );
//				DBGLOG( "\t\t|hasBump  %d\n", materialBlock.blinnBlock.lambert.bump.hasBump );
//				DBGLOG( "\t\t|bumpDepth  %.2f\n", materialBlock.blinnBlock.lambert.bump.bumpDepth );
//				DBGLOG( "\t\t|bumpIdx  %d\n", materialBlock.blinnBlock.lambert.bump.bumpIdx );
//
//				DBGLOG( "\t|specularColour  (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.blinnBlock.reflect.specularColour.R, materialBlock.blinnBlock.reflect.specularColour.G, materialBlock.blinnBlock.reflect.specularColour.B, materialBlock.blinnBlock.reflect.specularColour.A );
//				DBGLOG( "\t|specularColourIdx  %d\n", materialBlock.blinnBlock.reflect.specularColourIdx );
//				DBGLOG( "\t|reflectivity  %.2f\n", materialBlock.blinnBlock.reflect.reflectivity );
//				DBGLOG( "\t|reflectedColour  (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.blinnBlock.reflect.reflectedColour.R, materialBlock.blinnBlock.reflect.reflectedColour.G, materialBlock.blinnBlock.reflect.reflectedColour.B, materialBlock.blinnBlock.reflect.reflectedColour.A );
//
//				DBGLOG( "\t\t|hasEnvMap  (%d)\n", materialBlock.blinnBlock.reflect.envmapBlock.hasEnvMap );
//				DBGLOG( "\t\t|envType  (%d)\n", materialBlock.blinnBlock.reflect.envmapBlock.envType );
//				DBGLOG( "\t\t|envIdx  (%d,  %d,  %d,  %d,  %d,  %d)\n", materialBlock.blinnBlock.reflect.envmapBlock.envIdx[0], materialBlock.blinnBlock.reflect.envmapBlock.envIdx[1], materialBlock.blinnBlock.reflect.envmapBlock.envIdx[2], materialBlock.blinnBlock.reflect.envmapBlock.envIdx[3],
//																		materialBlock.blinnBlock.reflect.envmapBlock.envIdx[4], materialBlock.blinnBlock.reflect.envmapBlock.envIdx[5] );
//
//				DBGLOG( "\t|eccentricity  %.2f\n", materialBlock.blinnBlock.eccentricity );
//				DBGLOG( "\t|specularRollOff  %.2f\n", materialBlock.blinnBlock.specularRollOff );
//#endif // _DEBUG
			}
			else if( matType.eMatFormat == mdl::mat::MATTYPE_PHONG )
			{
				// read a phong block
				materialBlock.eMatFormat = mdl::mat::MATTYPE_PHONG;
				file::FileReadPhongBlock( &materialBlock.phongBlock, sizeof(mdl::mat::PhongShaderBlock), 1, &fileHandle );

				pMaterials[i].Ambient = materialBlock.phongBlock.lambert.ambientColour;
				//if( materialBlock.phongBlock.lambert.colourIdx == -1 )
				//	pMaterials[i].Diffuse = math::Vec4( materialBlock.phongBlock.lambert.colour.R*materialBlock.phongBlock.lambert.diffuseCoeff, materialBlock.phongBlock.lambert.colour.G*materialBlock.phongBlock.lambert.diffuseCoeff, materialBlock.phongBlock.lambert.colour.B*materialBlock.phongBlock.lambert.diffuseCoeff, 1.0f );
				//else
					pMaterials[i].Diffuse = math::Vec4( materialBlock.phongBlock.lambert.diffuseCoeff, materialBlock.phongBlock.lambert.diffuseCoeff, materialBlock.phongBlock.lambert.diffuseCoeff, 1.0f );
				if( materialBlock.phongBlock.reflect.specularColourIdx == -1 )
					pMaterials[i].Specular = materialBlock.phongBlock.reflect.specularColour;
				pMaterials[i].Emissive = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
				pMaterials[i].Shininess = materialBlock.phongBlock.cosPower;

				if( materialBlock.phongBlock.lambert.colourIdx != -1 )
				{
					pMaterials[i].texUnit[0].bEnabled = true;
					pMaterials[i].texUnit[0].nTexLookup = materialBlock.phongBlock.lambert.colourIdx;
					if( pMaterials[i].texUnit[0].nTexLookup != -1 )
						pMaterials[i].texUnit[0].nTextureID = m_TextureIDList[pMaterials[i].texUnit[0].nTexLookup];
					else
						pMaterials[i].texUnit[0].nTextureID = renderer::INVALID_OBJECT;
				}

				if( materialBlock.phongBlock.reflect.specularColourIdx != -1 )
				{
					pMaterials[i].texUnit[1].bEnabled = true;
					pMaterials[i].texUnit[1].nTexLookup = materialBlock.phongBlock.reflect.specularColourIdx;
					if( pMaterials[i].texUnit[1].nTexLookup != -1 )
						pMaterials[i].texUnit[1].nTextureID = m_TextureIDList[pMaterials[i].texUnit[1].nTexLookup];
					else
						pMaterials[i].texUnit[1].nTextureID = renderer::INVALID_OBJECT;

					pMaterials[i].Specular = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
				}

				if( materialBlock.phongBlock.lambert.bump.hasBump )
				{
					pMaterials[i].texUnit[2].bEnabled = true;
					pMaterials[i].texUnit[2].nTexLookup = materialBlock.phongBlock.lambert.bump.bumpIdx;
					if( pMaterials[i].texUnit[2].nTexLookup != -1 )
						pMaterials[i].texUnit[2].nTextureID = m_TextureIDList[pMaterials[i].texUnit[2].nTexLookup];
					else
						pMaterials[i].texUnit[2].nTextureID = renderer::INVALID_OBJECT;
				}

//#ifdef _DEBUG
//				DBGLOG( "Phong Block:\n" );
//				DBGLOG( "\t|diffuseCoeff  %.2f\n", materialBlock.phongBlock.lambert.diffuseCoeff );
//				DBGLOG( "\t|colour  (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.phongBlock.lambert.colour.R, materialBlock.phongBlock.lambert.colour.G, materialBlock.phongBlock.lambert.colour.B, materialBlock.phongBlock.lambert.colour.A );
//				DBGLOG( "\t|colourIdx  %d\n", materialBlock.phongBlock.lambert.colourIdx );
//				DBGLOG( "\t|transparency  (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.phongBlock.lambert.transparency.R, materialBlock.phongBlock.lambert.transparency.G, materialBlock.phongBlock.lambert.transparency.B, materialBlock.phongBlock.lambert.transparency.A );
//				DBGLOG( "\t|transparencyIdx  %d\n", materialBlock.phongBlock.lambert.transparencyIdx );
//				DBGLOG( "\t|ambientColour (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.phongBlock.lambert.ambientColour.R, materialBlock.phongBlock.lambert.ambientColour.G, materialBlock.phongBlock.lambert.ambientColour.B, materialBlock.phongBlock.lambert.ambientColour.A );
//				DBGLOG( "\t|ambientColourIdx  %d\n", materialBlock.phongBlock.lambert.ambientColourIdx );
//				DBGLOG( "\t|incandescence  (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.phongBlock.lambert.incandescence.R, materialBlock.phongBlock.lambert.incandescence.G, materialBlock.phongBlock.lambert.incandescence.B, materialBlock.phongBlock.lambert.incandescence.A );
//				DBGLOG( "\t|incandescenceIdx  %d\n", materialBlock.phongBlock.lambert.incandescenceIdx );
//				DBGLOG( "\t|translucenceCoeff  %.2f\n", materialBlock.phongBlock.lambert.translucenceCoeff );
//				DBGLOG( "\t|glowIntensity  %.2f\n", materialBlock.phongBlock.lambert.glowIntensity );
//				DBGLOG( "\t\t|hasBump  %d\n", materialBlock.phongBlock.lambert.bump.hasBump );
//				DBGLOG( "\t\t|bumpDepth  %.2f\n", materialBlock.phongBlock.lambert.bump.bumpDepth );
//				DBGLOG( "\t\t|bumpIdx  %d\n", materialBlock.phongBlock.lambert.bump.bumpIdx );
//
//				DBGLOG( "\t|specularColour  (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.phongBlock.reflect.specularColour.R, materialBlock.phongBlock.reflect.specularColour.G, materialBlock.phongBlock.reflect.specularColour.B, materialBlock.phongBlock.reflect.specularColour.A );
//				DBGLOG( "\t|specularColourIdx  %d\n", materialBlock.phongBlock.reflect.specularColourIdx );
//				DBGLOG( "\t|reflectivity  %.2f\n", materialBlock.phongBlock.reflect.reflectivity );
//				DBGLOG( "\t|reflectedColour  (%.2f,  %.2f,  %.2f,  %.2f)\n", materialBlock.phongBlock.reflect.reflectedColour.R, materialBlock.phongBlock.reflect.reflectedColour.G, materialBlock.phongBlock.reflect.reflectedColour.B, materialBlock.phongBlock.reflect.reflectedColour.A );
//
//				DBGLOG( "\t\t|hasEnvMap  (%d)\n", materialBlock.phongBlock.reflect.envmapBlock.hasEnvMap );
//				DBGLOG( "\t\t|envType  (%d)\n", materialBlock.phongBlock.reflect.envmapBlock.envType );
//				DBGLOG( "\t\t|envIdx  (%d,  %d,  %d,  %d,  %d,  %d)\n", materialBlock.phongBlock.reflect.envmapBlock.envIdx[0], materialBlock.phongBlock.reflect.envmapBlock.envIdx[1], materialBlock.phongBlock.reflect.envmapBlock.envIdx[2], materialBlock.phongBlock.reflect.envmapBlock.envIdx[3],
//																		materialBlock.phongBlock.reflect.envmapBlock.envIdx[4], materialBlock.phongBlock.reflect.envmapBlock.envIdx[5] );
//
//				DBGLOG( "\t|cosPower  %.2f\n", materialBlock.phongBlock.cosPower );
//#endif // _DEBUG

			}
			else if( matType.eMatFormat == mdl::mat::MATTYPE_SURFACESHADER )
			{
				materialBlock.eMatFormat = mdl::mat::MATTYPE_SURFACESHADER;
				int textureIndex = -1;
				file::FileReadInt( &textureIndex, sizeof(int), 1, &fileHandle );

				pMaterials[i].Ambient = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
				pMaterials[i].Diffuse = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
				pMaterials[i].Specular = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
				pMaterials[i].Emissive = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
				pMaterials[i].Shininess = 0.0f;

				if( materialBlock.phongBlock.lambert.colourIdx != -1 )
				{
					pMaterials[i].texUnit[0].bEnabled = true;
					pMaterials[i].texUnit[0].nTexLookup = textureIndex;
					if( pMaterials[i].texUnit[0].nTexLookup != -1 )
						pMaterials[i].texUnit[0].nTextureID = m_TextureIDList[pMaterials[i].texUnit[0].nTexLookup];
					else
						pMaterials[i].texUnit[0].nTextureID = renderer::INVALID_OBJECT;
				}
			}

		}
		else
		{
			// create generic material
			DBGLOG( "MODELHGM: *WARNING* Unsupported material\n" );

			//materialBlock.eMatFormat = mdl::mat::MATTYPE_LAMBERT;
			//materialBlock.lambertBlock = lambertDefault;
		}

		// add to the tracking list
		m_MaterialBlockList.push_back( materialBlock );
	}
	
	file::FileReadUInt( &bHasVertices, sizeof( int ), 1, &fileHandle );
	file::FileReadUInt( &bHasTextureCoords, sizeof( int ), 1, &fileHandle );
	file::FileReadUInt( &bHasNormals, sizeof( int ), 1, &fileHandle );
	file::FileReadUInt( &bSortMeshes, sizeof( int ), 1, &fileHandle );

	file::FileReadUInt( &nNumOpaqueMeshes, sizeof( unsigned int ), 1, &fileHandle );
	file::FileReadUInt( &nNumSortedMeshes, sizeof( unsigned int ), 1, &fileHandle );

	// mesh lists
	if( nNumOpaqueMeshes )
	{
		DBG_MEMTRY
			pOpaqueMeshList = new Mesh[nNumOpaqueMeshes];
		DBG_MEMCATCH
	}

	if( nNumSortedMeshes )
	{
		DBG_MEMTRY
			pSortedMeshList = new Mesh[nNumSortedMeshes];
		DBG_MEMCATCH
	}

	int opaqueIndex = 0;
	int sortedIndex = 0;

	Mesh* pMesh = 0;
	
	for( i = 0; i < nNumOpaqueMeshes+nNumSortedMeshes; i++ )
	{
		// sort flag
		int sortMesh = false;
		file::FileReadInt( &sortMesh, sizeof( int ), 1, &fileHandle );

		// work out which list to add this mesh to
		if( sortMesh )
			pMesh = &pSortedMeshList[sortedIndex];
		else
			pMesh =	&pOpaqueMeshList[opaqueIndex];

		// start read
		pMesh->Initialise();
		
		pMesh->meshSort = sortMesh;

		pMesh->nMeshId = i;

		file::FileReadVec3D( &pMesh->meshTransformData.translate, sizeof(math::Vec3), 1, &fileHandle );
		file::FileReadVec3D( &pMesh->meshTransformData.eulerRotation, sizeof(math::Vec3), 1, &fileHandle );
		file::FileReadVec3D( &pMesh->meshTransformData.scale, sizeof(math::Vec3), 1, &fileHandle );

		file::FileReadVec3D( &pMesh->meshAABB.vCenter, sizeof(math::Vec3), 1, &fileHandle );
		file::FileReadVec3D( &pMesh->meshAABB.vBoxMin, sizeof(math::Vec3), 1, &fileHandle );
		file::FileReadVec3D( &pMesh->meshAABB.vBoxMax, sizeof(math::Vec3), 1, &fileHandle );

		//pMesh->meshAABB.vCenter = pMesh->meshTransformData.translate;

		file::FileReadInt( &pMesh->nMeshIndex, sizeof( int ), 1, &fileHandle );

		// does the file contain fog settings
		int hasFogData = false;
		file::FileReadInt( &hasFogData, sizeof( int ), 1, &fileHandle );
		if( hasFogData )
		{
			file::FileReadUInt( &pMesh->meshFogState.bFogState, sizeof( int ), 1, &fileHandle );
			file::FileReadUInt( &pMesh->meshFogState.eFogMode, sizeof( int ), 1, &fileHandle );
			file::FileReadVec3D( &pMesh->meshFogState.vFogColour, sizeof( math::Vec3 ), 1, &fileHandle );
			file::FileReadFloat( &pMesh->meshFogState.fFogNearClip, sizeof( float ), 1, &fileHandle );
			file::FileReadFloat( &pMesh->meshFogState.fFogFarClip, sizeof( float ), 1, &fileHandle );
			file::FileReadFloat( &pMesh->meshFogState.fFogDensity, sizeof( float ), 1, &fileHandle );

			if( pMesh->meshFogState.bFogState == true )
				bUseSubMeshFog = true;
		}

		file::FileReadInt( &pMesh->meshSimpleColourFlag, sizeof( int ), 1, &fileHandle );
		if( pMesh->meshSimpleColourFlag )
		{
			int col[4];
			file::FileReadInt( &col[0], sizeof( int ), 4, &fileHandle );

			pMesh->meshSimpleColour = math::Vec4Lite( col[0], col[1], col[2], col[3] );
		}

		// per mesh enabled data
		pMesh->bHasVertices		= bHasVertices;
		pMesh->bHasTextureCoords	= bHasTextureCoords;
		pMesh->bHasNormals		= bHasNormals;

		file::FileReadUInt( &pMesh->nNumMaterialTriangleLists, sizeof( unsigned int ), 1, &fileHandle );

		// allocate per mesh data
		DBG_MEMTRY
			pMesh->pMaterials = new TMaterialEx[ pMesh->nNumMaterialTriangleLists ];
		DBG_MEMCATCH

		DBG_MEMTRY
			pMesh->pMaterialIndexList = new int[ pMesh->nNumMaterialTriangleLists ];
		DBG_MEMCATCH

		DBG_MEMTRY
			pMesh->pTriPerMaterialList = new unsigned int[ pMesh->nNumMaterialTriangleLists ];
		DBG_MEMCATCH

		for( j = 0; j < pMesh->nNumMaterialTriangleLists; j++ )
		{
			file::FileReadInt( &pMesh->pMaterialIndexList[j], sizeof( int ), 1, &fileHandle );
			file::FileReadUInt( &pMesh->pTriPerMaterialList[j], sizeof( unsigned int ), 1, &fileHandle );

			// assign main materials to sub mesh material
			if( pMesh->pMaterialIndexList[j] >= 0 && pMesh->pMaterialIndexList[j] < static_cast<int>(nNumMaterials) )
				std::memcpy( &pMesh->pMaterials[j], &pMaterials[ pMesh->pMaterialIndexList[j] ], sizeof( TMaterialEx ) );
			else
				std::memset( &pMesh->pMaterials[j], 0, sizeof( TMaterialEx ) );
		}
		
		file::FileReadUInt( &pMesh->nTotalPolyCount, sizeof( unsigned int ), 1, &fileHandle );
		file::FileReadUInt( &pMesh->nNumVertices, sizeof( unsigned int ), 1, &fileHandle );
		file::FileReadUInt( &pMesh->nTotalTriangleCount, sizeof( unsigned int ), 1, &fileHandle );

		// add this to the inherited Model values
		nNumVertices += pMesh->nNumVertices;
		m_TotalPolyCount += pMesh->nTotalPolyCount;
		m_TotalTriCount += pMesh->nTotalTriangleCount;

		pMesh->nTotalVertexCount = pMesh->nNumVertices;

		DBG_MEMTRY
			pMesh->pInterleavedData = new InterleavedData[pMesh->nTotalVertexCount];
		DBG_MEMCATCH

		int nIndex = 0;

		// for each material in this mesh...
		for( j = 0; j < pMesh->nNumMaterialTriangleLists; j++ )
		{
			// ...and for each triangle in this material
			for( k = 0; k < pMesh->pTriPerMaterialList[j]; k++ )
			{
				// assign the verts, texcoords, normals and vert colours
				if( bHasVertices )
				{
					file::FileReadFloat( &pMesh->pInterleavedData[nIndex].v[0], sizeof(float), 3, &fileHandle );
					file::FileReadFloat( &pMesh->pInterleavedData[nIndex+1].v[0], sizeof(float), 3, &fileHandle );
					file::FileReadFloat( &pMesh->pInterleavedData[nIndex+2].v[0], sizeof(float), 3, &fileHandle );
				}
				if( bHasTextureCoords )
				{
					file::FileReadFloat( &pMesh->pInterleavedData[nIndex].uv[0], sizeof(float), 2, &fileHandle );	
					file::FileReadFloat( &pMesh->pInterleavedData[nIndex+1].uv[0], sizeof(float), 2, &fileHandle );
					file::FileReadFloat( &pMesh->pInterleavedData[nIndex+2].uv[0], sizeof(float), 2, &fileHandle );

					/*for( int l=0; l<2; ++l)
					{
						pMesh->pInterleavedData[nIndex].uv1[l] = pMesh->pInterleavedData[nIndex].uv[l];
						pMesh->pInterleavedData[nIndex+1].uv1[l] = pMesh->pInterleavedData[nIndex+1].uv[l];
						pMesh->pInterleavedData[nIndex+2].uv1[l] = pMesh->pInterleavedData[nIndex+2].uv[l];
					}*/
				}
				if( bHasNormals )
				{
					file::FileReadFloat( &pMesh->pInterleavedData[nIndex].n[0], sizeof(float), 3, &fileHandle );
					file::FileReadFloat( &pMesh->pInterleavedData[nIndex+1].n[0], sizeof(float), 3, &fileHandle );
					file::FileReadFloat( &pMesh->pInterleavedData[nIndex+2].n[0], sizeof(float), 3, &fileHandle );
				}

				nIndex += 3;
			}
		}

		pMesh->CalculateBounds();

		float fNewRadius = 0.0f;

		// check the sub mesh box against the main mesh box
		if( pMesh->meshAABB.vBoxMin.X < modelAABB.vBoxMin.X )
			modelAABB.vBoxMin.X= pMesh->meshAABB.vBoxMin.X ;

		if( pMesh->meshAABB.vBoxMin.Y < modelAABB.vBoxMin.Y )
			modelAABB.vBoxMin.Y = pMesh->meshAABB.vBoxMin.Y;

		if( pMesh->meshAABB.vBoxMin.Z < modelAABB.vBoxMin.Z )
			modelAABB.vBoxMin.Z = pMesh->meshAABB.vBoxMin.Z;


		if( pMesh->meshAABB.vBoxMax.X > modelAABB.vBoxMax.X )
			modelAABB.vBoxMax.X  = pMesh->meshAABB.vBoxMax.X;

		if( pMesh->meshAABB.vBoxMax.Y > modelAABB.vBoxMax.Y )
			modelAABB.vBoxMax.Y = pMesh->meshAABB.vBoxMax.Y;

		if( pMesh->meshAABB.vBoxMax.Z > modelAABB.vBoxMax.Z )
			modelAABB.vBoxMax.Z = pMesh->meshAABB.vBoxMax.Z;

		// BOUNDING SPHERE
		// get a center point based off the box size above
		modelSphere.vCenterPoint.X = modelAABB.vBoxMax.X - ((modelAABB.vBoxMax.X - modelAABB.vBoxMin.X)*0.5f);
		modelSphere.vCenterPoint.Y = modelAABB.vBoxMax.Y - ((modelAABB.vBoxMax.Y - modelAABB.vBoxMin.Y)*0.5f);
		modelSphere.vCenterPoint.Z = modelAABB.vBoxMax.Z - ((modelAABB.vBoxMax.Z - modelAABB.vBoxMin.Z)*0.5f);

		// line lengths (0.75 takes away the over safe full size radius and wraps it around the box nicely)
		float fLineX = (modelAABB.vBoxMax.X - modelAABB.vBoxMin.X)*0.75f;
		float fLineY = (modelAABB.vBoxMax.Y - modelAABB.vBoxMin.Y)*0.75f;
		float fLineZ = (modelAABB.vBoxMax.Z - modelAABB.vBoxMin.Z)*0.75f;

		// get the SMALLEST difference to get as close to the mesh edge
		if( fLineX > fNewRadius )
			fNewRadius = fLineX;	

		if( fLineY > fNewRadius )
			fNewRadius = fLineY;
	
		if( fLineZ > fNewRadius )
			fNewRadius = fLineZ;

		modelSphere.fRadius = fNewRadius;

		pMesh->bValidLoad = true;

		if( sortMesh )
			sortedIndex++;
		else
			opaqueIndex++;
	}

	file::FileClose( &fileHandle );

	if( bSortMeshes )
		std::qsort( pOpaqueMeshList, GetNumOpaqueMeshes(), sizeof(Mesh), MeshSort );

	bValidLoad = true;

	//DBGLOG( "MODELHGM: MODEL %s file loaded\n", szFilename );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: SetupAndFree
/// Params: None
/// 
/////////////////////////////////////////////////////
void ModelHGM::SetupAndFree()
{
	if( !bValidLoad  )
		return;

	if( core::app::GetUseVertexArrays() ||
		!renderer::bExtVertexBufferObjects)
		return;

	unsigned int i = 0;
	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		if( pOpaqueMeshList )
		{
			// automatically frees the local memory after upload
			pOpaqueMeshList[i].SetupVertexBufferObject();
		}
	}

	for( i = 0; i < nNumSortedMeshes; i++ )
	{
		if( pSortedMeshList )
		{
			// automatically frees the local memory after upload
			pSortedMeshList[i].SetupVertexBufferObject();
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
/// 
/////////////////////////////////////////////////////
void ModelHGM::Draw( void )
{
	if( !bValidLoad  )
		return;

	unsigned int i = 0;
	math::Vec4Lite origColour;
	bool lightingState = renderer::OpenGL::GetInstance()->GetLightingState();

	if( bHasNormals )
		renderer::OpenGL::GetInstance()->EnableNormalArray();

	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		//glPushMatrix();
		//renderer::DrawAABB( pOpaqueMeshList[i].meshAABB.vBoxMin, pOpaqueMeshList[i].meshAABB.vBoxMax);
		//	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
		//glPopMatrix();

		if( pOpaqueMeshList && pOpaqueMeshList[i].bDraw )
		{

			if( pOpaqueMeshList[i].meshSimpleColourFlag )
			{
				lightingState = renderer::OpenGL::GetInstance()->GetLightingState();

				if( lightingState )
					renderer::OpenGL::GetInstance()->DisableLighting();

				origColour = renderer::OpenGL::GetInstance()->GetColour4ub();
				renderer::OpenGL::GetInstance()->SetColour4ub( pOpaqueMeshList[i].meshSimpleColour.R, pOpaqueMeshList[i].meshSimpleColour.G, pOpaqueMeshList[i].meshSimpleColour.B, pOpaqueMeshList[i].meshSimpleColour.A );
			}
			else if( m_UseDefaultColour )
				renderer::OpenGL::GetInstance()->SetColour4ub( m_DefaultMeshColour.R, m_DefaultMeshColour.G, m_DefaultMeshColour.B, m_DefaultMeshColour.A );

			if( nGeneralFlags & MODELFLAG_GENERAL_AUTOCULLING )
			{
				if( nGeneralFlags & MODELFLAG_GENERAL_SPHERECULL )
				{
					collision::Sphere sphere = pOpaqueMeshList[i].meshSphere;

					if( nGeneralFlags & MODELFLAG_GENERAL_MODELCULL )
					{
						sphere = modelSphere;
					}
					else if( nGeneralFlags & MODELFLAG_GENERAL_SUBMESHCULL )
					{
						sphere = pOpaqueMeshList[i].meshSphere;
					}

					if( renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
					{
						if( pOpaqueMeshList[i].meshTranslate || 
							pOpaqueMeshList[i].meshRotate ||
							pOpaqueMeshList[i].meshScale )
						{
							glPushMatrix();
							if( pOpaqueMeshList[i].meshTranslate )
								glTranslatef( pOpaqueMeshList[i].meshTranslation.X, pOpaqueMeshList[i].meshTranslation.Y, pOpaqueMeshList[i].meshTranslation.Z );

							if( pOpaqueMeshList[i].meshScale )
							{
								glScalef( pOpaqueMeshList[i].meshScaleFactors.X, pOpaqueMeshList[i].meshScaleFactors.Y, pOpaqueMeshList[i].meshScaleFactors.Z );
							}
							if( pOpaqueMeshList[i].meshRotate )
							{
								glRotatef( pOpaqueMeshList[i].meshRotation.X, 1.0f, 0.0f, 0.0f );
								glRotatef( pOpaqueMeshList[i].meshRotation.Y, 0.0f, 1.0f, 0.0f );
								glRotatef( pOpaqueMeshList[i].meshRotation.Z, 0.0f, 0.0f, 1.0f );
							}

						}

						// draw the mesh
						pOpaqueMeshList[i].Draw();

						if( pOpaqueMeshList[i].meshTranslate ||
							pOpaqueMeshList[i].meshRotate ||
							pOpaqueMeshList[i].meshScale )
						{
							glPopMatrix();
						}

						if( renderer::OpenGL::IsInitialised() )
						{
							renderer::OpenGL::GetInstance()->nTotalTriangleCount += pOpaqueMeshList[i].nTotalTriangleCount;
							renderer::OpenGL::GetInstance()->nTotalVertexCount += pOpaqueMeshList[i].nTotalVertexCount;
						}
					}
				}

				if( nGeneralFlags & MODELFLAG_GENERAL_AABBCULL )
				{
					collision::AABB aabb = pOpaqueMeshList[i].meshAABB;

					if( nGeneralFlags & MODELFLAG_GENERAL_MODELCULL )
					{
						aabb = modelAABB;
					}
					else if( nGeneralFlags & MODELFLAG_GENERAL_SUBMESHCULL )
					{
						aabb = pOpaqueMeshList[i].meshAABB;
					}

					if( renderer::OpenGL::GetInstance()->AABBInFrustum( aabb ) )
					{
						if( pOpaqueMeshList[i].meshTranslate || 
							pOpaqueMeshList[i].meshRotate ||
							pOpaqueMeshList[i].meshScale )
						{
							glPushMatrix();
							if( pOpaqueMeshList[i].meshTranslate )
								glTranslatef( pOpaqueMeshList[i].meshTranslation.X, pOpaqueMeshList[i].meshTranslation.Y, pOpaqueMeshList[i].meshTranslation.Z );
							if( pOpaqueMeshList[i].meshScale )
							{
								glScalef( pOpaqueMeshList[i].meshScaleFactors.X, pOpaqueMeshList[i].meshScaleFactors.Y, pOpaqueMeshList[i].meshScaleFactors.Z );
							}
							if( pOpaqueMeshList[i].meshRotate )
							{
								glRotatef( pOpaqueMeshList[i].meshRotation.X, 1.0f, 0.0f, 0.0f );
								glRotatef( pOpaqueMeshList[i].meshRotation.Y, 0.0f, 1.0f, 0.0f );
								glRotatef( pOpaqueMeshList[i].meshRotation.Z, 0.0f, 0.0f, 1.0f );
							}

						}

						// draw the mesh
						pOpaqueMeshList[i].Draw();

						if( pOpaqueMeshList[i].meshTranslate ||
							pOpaqueMeshList[i].meshRotate ||
							pOpaqueMeshList[i].meshScale )
						{
							glPopMatrix();
						}

						if( renderer::OpenGL::IsInitialised() )
						{
							renderer::OpenGL::GetInstance()->nTotalTriangleCount += pOpaqueMeshList[i].nTotalTriangleCount;
							renderer::OpenGL::GetInstance()->nTotalVertexCount += pOpaqueMeshList[i].nTotalVertexCount;
						}
					}
				}
			}
			else
			{
				if( pOpaqueMeshList[i].meshTranslate || 
					pOpaqueMeshList[i].meshRotate ||
					pOpaqueMeshList[i].meshScale )
				{
					glPushMatrix();
					if( pOpaqueMeshList[i].meshTranslate )
						glTranslatef( pOpaqueMeshList[i].meshTranslation.X, pOpaqueMeshList[i].meshTranslation.Y, pOpaqueMeshList[i].meshTranslation.Z );
					if( pOpaqueMeshList[i].meshScale )
					{
						glScalef( pOpaqueMeshList[i].meshScaleFactors.X, pOpaqueMeshList[i].meshScaleFactors.Y, pOpaqueMeshList[i].meshScaleFactors.Z );
					}
					if( pOpaqueMeshList[i].meshRotate )
					{
						glRotatef( pOpaqueMeshList[i].meshRotation.X, 1.0f, 0.0f, 0.0f );
						glRotatef( pOpaqueMeshList[i].meshRotation.Y, 0.0f, 1.0f, 0.0f );
						glRotatef( pOpaqueMeshList[i].meshRotation.Z, 0.0f, 0.0f, 1.0f );
					}

				}

				// draw the mesh
				pOpaqueMeshList[i].Draw();

				if( pOpaqueMeshList[i].meshTranslate ||
					pOpaqueMeshList[i].meshRotate ||
					pOpaqueMeshList[i].meshScale )
				{
					glPopMatrix();
				}
			
				if( renderer::OpenGL::IsInitialised() )
				{
					renderer::OpenGL::GetInstance()->nTotalTriangleCount += pOpaqueMeshList[i].nTotalTriangleCount;
					renderer::OpenGL::GetInstance()->nTotalVertexCount += pOpaqueMeshList[i].nTotalVertexCount;
				}
			}

			// reset colour
			if( pOpaqueMeshList[i].meshSimpleColourFlag )
			{
				if( lightingState )
					renderer::OpenGL::GetInstance()->EnableLighting();

				renderer::OpenGL::GetInstance()->SetColour4ub( origColour.R, origColour.G, origColour.B, origColour.A );
			}
		}
	}

	if( bHasNormals )
		renderer::OpenGL::GetInstance()->DisableNormalArray();

	// disable any VBO arrays
	renderer::OpenGL::GetInstance()->DisableVBO();
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
/// 
/////////////////////////////////////////////////////
void ModelHGM::Draw( bool sortedMeshes )
{
	if( !bValidLoad  )
		return;

	unsigned int i = 0;
	unsigned int maxCount = 0;
	bool lightingState = renderer::OpenGL::GetInstance()->GetLightingState();
	math::Vec4Lite origColour;
	Mesh* pMesh = 0;

	if( sortedMeshes )
	{
		maxCount = nNumSortedMeshes;
		//renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );
	}
	else
		maxCount = nNumOpaqueMeshes;

	if( bHasNormals )
		renderer::OpenGL::GetInstance()->EnableNormalArray();

	for( i = 0; i < maxCount; i++ )
	{
		if( sortedMeshes )
			pMesh = &pSortedMeshList[i];
		else
			pMesh = &pOpaqueMeshList[i];

		//if( i == 10 )
		//	continue;
		//{
		//	glPushMatrix();
		//		renderer::DrawAABB( pMesh->meshAABB.vBoxMin, pMesh->meshAABB.vBoxMax);
		//		renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
		//	glPopMatrix();
		//}
		
		if( pMesh && pMesh->bDraw )
		{

			if( pMesh->meshSimpleColourFlag )
			{
				lightingState = renderer::OpenGL::GetInstance()->GetLightingState();

				if( lightingState )
					renderer::OpenGL::GetInstance()->DisableLighting();

				origColour = renderer::OpenGL::GetInstance()->GetColour4ub();
				renderer::OpenGL::GetInstance()->SetColour4ub( pMesh->meshSimpleColour.R, pMesh->meshSimpleColour.G, pMesh->meshSimpleColour.B, pMesh->meshSimpleColour.A );
			}
			else if( m_UseDefaultColour )
				renderer::OpenGL::GetInstance()->SetColour4ub( m_DefaultMeshColour.R, m_DefaultMeshColour.G, m_DefaultMeshColour.B, m_DefaultMeshColour.A );

			if( nGeneralFlags & MODELFLAG_GENERAL_AUTOCULLING )
			{
				if( nGeneralFlags & MODELFLAG_GENERAL_SPHERECULL )
				{
					collision::Sphere sphere = pMesh->meshSphere;

					if( nGeneralFlags & MODELFLAG_GENERAL_MODELCULL )
					{
						sphere = modelSphere;
					}
					else if( nGeneralFlags & MODELFLAG_GENERAL_SUBMESHCULL )
					{
						sphere = pMesh->meshSphere;
					}

					if( renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
					{
						if( pMesh->meshTranslate || 
							pMesh->meshRotate ||
							pMesh->meshScale )
						{
							glPushMatrix();
							if( pMesh->meshTranslate )
								glTranslatef( pMesh->meshTranslation.X, pMesh->meshTranslation.Y, pMesh->meshTranslation.Z );
							if( pMesh->meshScale )
							{
								glScalef( pMesh->meshScaleFactors.X, pMesh->meshScaleFactors.Y, pMesh->meshScaleFactors.Z );
							}
							if( pMesh->meshRotate )
							{
								glRotatef( pMesh->meshRotation.X, 1.0f, 0.0f, 0.0f );
								glRotatef( pMesh->meshRotation.Y, 0.0f, 1.0f, 0.0f );
								glRotatef( pMesh->meshRotation.Z, 0.0f, 0.0f, 1.0f );
							}

						}


						// draw the mesh
						pMesh->Draw();
					
						if( pMesh->meshTranslate ||
							pMesh->meshRotate ||
							pMesh->meshScale )
						{
							glPopMatrix();
						}

						if( renderer::OpenGL::IsInitialised() )
						{
							renderer::OpenGL::GetInstance()->nTotalTriangleCount += pMesh->nTotalTriangleCount;
							renderer::OpenGL::GetInstance()->nTotalVertexCount += pMesh->nTotalVertexCount;
						}
					}
				}

				if( nGeneralFlags & MODELFLAG_GENERAL_AABBCULL )
				{
					collision::AABB aabb = pMesh->meshAABB;

					if( nGeneralFlags & MODELFLAG_GENERAL_MODELCULL )
					{
						aabb = modelAABB;
					}
					else if( nGeneralFlags & MODELFLAG_GENERAL_SUBMESHCULL )
					{
						aabb = pMesh->meshAABB;
					}

					if( renderer::OpenGL::GetInstance()->AABBInFrustum( aabb ) )
					{
						if( pMesh->meshTranslate || 
							pMesh->meshRotate ||
							pMesh->meshScale )
						{
							glPushMatrix();
							if( pMesh->meshTranslate )
								glTranslatef( pMesh->meshTranslation.X, pMesh->meshTranslation.Y, pMesh->meshTranslation.Z );
							if( pMesh->meshScale )
							{
								glScalef( pMesh->meshScaleFactors.X, pMesh->meshScaleFactors.Y, pMesh->meshScaleFactors.Z );
							}
							if( pMesh->meshRotate )
							{
								glRotatef( pMesh->meshRotation.X, 1.0f, 0.0f, 0.0f );
								glRotatef( pMesh->meshRotation.Y, 0.0f, 1.0f, 0.0f );
								glRotatef( pMesh->meshRotation.Z, 0.0f, 0.0f, 1.0f );
							}

						}

						// draw the mesh
						pMesh->Draw();

						if( pMesh->meshTranslate ||
							pMesh->meshRotate ||
							pMesh->meshScale )
						{
							glPopMatrix();
						}

						if( renderer::OpenGL::IsInitialised() )
						{
							renderer::OpenGL::GetInstance()->nTotalTriangleCount += pMesh->nTotalTriangleCount;
							renderer::OpenGL::GetInstance()->nTotalVertexCount += pMesh->nTotalVertexCount;
						}
					}
				}
			}
			else
			{
				pMesh->Draw();
			
				if( renderer::OpenGL::IsInitialised() )
				{
					renderer::OpenGL::GetInstance()->nTotalTriangleCount += pMesh->nTotalTriangleCount;
					renderer::OpenGL::GetInstance()->nTotalVertexCount += pMesh->nTotalVertexCount;
				}
			}

			if( pMesh->meshSimpleColourFlag )
			{
				if( lightingState )
					renderer::OpenGL::GetInstance()->EnableLighting();

				renderer::OpenGL::GetInstance()->SetColour4ub( origColour.R, origColour.G, origColour.B, origColour.A );
			}
		}
	}

	//if( sortedMeshes )
	//	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );

	if( bHasNormals )
		renderer::OpenGL::GetInstance()->DisableNormalArray();

	// disable any VBO arrays
	renderer::OpenGL::GetInstance()->DisableVBO();
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: [in]fromIndex, [in]toIndex
///
/////////////////////////////////////////////////////
void ModelHGM::Draw( int fromIndex, int toIndex )
{
	if( !bValidLoad  )
		return;

	unsigned int i = 0;
	math::Vec4Lite origColour;

	if( bHasNormals )
		renderer::OpenGL::GetInstance()->EnableNormalArray();

	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		if( pOpaqueMeshList[i].nMeshIndex < fromIndex ||
			pOpaqueMeshList[i].nMeshIndex > toIndex )
			continue;

		if( pOpaqueMeshList && pOpaqueMeshList[i].bDraw )
		{

			if( pOpaqueMeshList[i].meshSimpleColourFlag )
			{
				origColour = renderer::OpenGL::GetInstance()->GetColour4ub();
				renderer::OpenGL::GetInstance()->SetColour4ub( pOpaqueMeshList[i].meshSimpleColour.R, pOpaqueMeshList[i].meshSimpleColour.G, pOpaqueMeshList[i].meshSimpleColour.B, pOpaqueMeshList[i].meshSimpleColour.A );
			}
			else if( m_UseDefaultColour )
				renderer::OpenGL::GetInstance()->SetColour4ub( m_DefaultMeshColour.R, m_DefaultMeshColour.G, m_DefaultMeshColour.B, m_DefaultMeshColour.A );

			// save old state
			if( pOpaqueMeshList[i].meshFogState.bFogState )
			{
				renderer::OpenGL::GetInstance()->SaveFogState( &pOpaqueMeshList[i].savedFogState );
				// set new state
				renderer::OpenGL::GetInstance()->SetFogState( &pOpaqueMeshList[i].meshFogState );
			}

			if( nGeneralFlags & MODELFLAG_GENERAL_AUTOCULLING )
			{
				if( nGeneralFlags & MODELFLAG_GENERAL_SPHERECULL )
				{
					collision::Sphere sphere = pOpaqueMeshList[i].meshSphere;

					if( nGeneralFlags & MODELFLAG_GENERAL_MODELCULL )
					{
						sphere = modelSphere;
					}
					else if( nGeneralFlags & MODELFLAG_GENERAL_SUBMESHCULL )
					{
						sphere = pOpaqueMeshList[i].meshSphere;
					}

					if( renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
					{
						if( pOpaqueMeshList[i].meshTranslate || 
							pOpaqueMeshList[i].meshRotate ||
							pOpaqueMeshList[i].meshScale )
						{
							glPushMatrix();
							if( pOpaqueMeshList[i].meshTranslate )
								glTranslatef( pOpaqueMeshList[i].meshTranslation.X, pOpaqueMeshList[i].meshTranslation.Y, pOpaqueMeshList[i].meshTranslation.Z );
							if( pOpaqueMeshList[i].meshScale )
							{
								glScalef( pOpaqueMeshList[i].meshScaleFactors.X, pOpaqueMeshList[i].meshScaleFactors.Y, pOpaqueMeshList[i].meshScaleFactors.Z );
							}
							if( pOpaqueMeshList[i].meshRotate )
							{
								glRotatef( pOpaqueMeshList[i].meshRotation.X, 1.0f, 0.0f, 0.0f );
								glRotatef( pOpaqueMeshList[i].meshRotation.Y, 0.0f, 1.0f, 0.0f );
								glRotatef( pOpaqueMeshList[i].meshRotation.Z, 0.0f, 0.0f, 1.0f );
							}

						}

						// draw the mesh
						pOpaqueMeshList[i].Draw();

						if( pOpaqueMeshList[i].meshTranslate ||
							pOpaqueMeshList[i].meshRotate ||
							pOpaqueMeshList[i].meshScale )
						{
							glPopMatrix();
						}
					
						if( renderer::OpenGL::IsInitialised() )
						{
							renderer::OpenGL::GetInstance()->nTotalTriangleCount += pOpaqueMeshList[i].nTotalTriangleCount;
							renderer::OpenGL::GetInstance()->nTotalVertexCount += pOpaqueMeshList[i].nTotalVertexCount;
						}
					}
				}

				if( nGeneralFlags & MODELFLAG_GENERAL_AABBCULL )
				{
					collision::AABB aabb = pOpaqueMeshList[i].meshAABB;

					if( nGeneralFlags & MODELFLAG_GENERAL_MODELCULL )
					{
						aabb = modelAABB;
					}
					else if( nGeneralFlags & MODELFLAG_GENERAL_SUBMESHCULL )
					{
						aabb = pOpaqueMeshList[i].meshAABB;
					}

					if( renderer::OpenGL::GetInstance()->AABBInFrustum( aabb ) )
					{
						if( pOpaqueMeshList[i].meshTranslate || 
							pOpaqueMeshList[i].meshRotate ||
							pOpaqueMeshList[i].meshScale )
						{
							glPushMatrix();
							if( pOpaqueMeshList[i].meshTranslate )
								glTranslatef( pOpaqueMeshList[i].meshTranslation.X, pOpaqueMeshList[i].meshTranslation.Y, pOpaqueMeshList[i].meshTranslation.Z );
							if( pOpaqueMeshList[i].meshScale )
							{
								glScalef( pOpaqueMeshList[i].meshScaleFactors.X, pOpaqueMeshList[i].meshScaleFactors.Y, pOpaqueMeshList[i].meshScaleFactors.Z );
							}
							if( pOpaqueMeshList[i].meshRotate )
							{
								glRotatef( pOpaqueMeshList[i].meshRotation.X, 1.0f, 0.0f, 0.0f );
								glRotatef( pOpaqueMeshList[i].meshRotation.Y, 0.0f, 1.0f, 0.0f );
								glRotatef( pOpaqueMeshList[i].meshRotation.Z, 0.0f, 0.0f, 1.0f );
							}

						}

						// draw the mesh
						pOpaqueMeshList[i].Draw();

						if( pOpaqueMeshList[i].meshTranslate ||
							pOpaqueMeshList[i].meshRotate ||
							pOpaqueMeshList[i].meshScale )
						{
							glPopMatrix();
						}

						if( renderer::OpenGL::IsInitialised() )
						{
							renderer::OpenGL::GetInstance()->nTotalTriangleCount += pOpaqueMeshList[i].nTotalTriangleCount;
							renderer::OpenGL::GetInstance()->nTotalVertexCount += pOpaqueMeshList[i].nTotalVertexCount;
						}
					}
				}
			}
			else
			{
				if( pOpaqueMeshList[i].meshTranslate || 
					pOpaqueMeshList[i].meshRotate ||
					pOpaqueMeshList[i].meshScale )
				{
					glPushMatrix();
					if( pOpaqueMeshList[i].meshTranslate )
						glTranslatef( pOpaqueMeshList[i].meshTranslation.X, pOpaqueMeshList[i].meshTranslation.Y, pOpaqueMeshList[i].meshTranslation.Z );
					if( pOpaqueMeshList[i].meshScale )
					{
						glScalef( pOpaqueMeshList[i].meshScaleFactors.X, pOpaqueMeshList[i].meshScaleFactors.Y, pOpaqueMeshList[i].meshScaleFactors.Z );
					}
					if( pOpaqueMeshList[i].meshRotate )
					{
						glRotatef( pOpaqueMeshList[i].meshRotation.X, 1.0f, 0.0f, 0.0f );
						glRotatef( pOpaqueMeshList[i].meshRotation.Y, 0.0f, 1.0f, 0.0f );
						glRotatef( pOpaqueMeshList[i].meshRotation.Z, 0.0f, 0.0f, 1.0f );
					}

				}

				// draw the mesh
				pOpaqueMeshList[i].Draw();
			
				if( pOpaqueMeshList[i].meshTranslate ||
					pOpaqueMeshList[i].meshRotate ||
					pOpaqueMeshList[i].meshScale )
				{
					glPopMatrix();
				}

				if( renderer::OpenGL::IsInitialised() )
				{
					renderer::OpenGL::GetInstance()->nTotalTriangleCount += pOpaqueMeshList[i].nTotalTriangleCount;
					renderer::OpenGL::GetInstance()->nTotalVertexCount += pOpaqueMeshList[i].nTotalVertexCount;
				}
			}

			if( pOpaqueMeshList[i].meshSimpleColourFlag )
			{
				renderer::OpenGL::GetInstance()->SetColour4ub( origColour.R, origColour.G, origColour.B, origColour.A );
			}

			if( pOpaqueMeshList[i].meshFogState.bFogState )
			{
				// restore fog state
				renderer::OpenGL::GetInstance()->SetFogState( &pOpaqueMeshList[i].savedFogState );
			}
		}
	}

	if( bHasNormals )
		renderer::OpenGL::GetInstance()->DisableNormalArray();

	// disable any VBO arrays
	renderer::OpenGL::GetInstance()->DisableVBO();
}

/////////////////////////////////////////////////////
/// Method: DrawMeshId
/// Params: [in]meshId
///
/////////////////////////////////////////////////////
void ModelHGM::DrawMeshId( unsigned int meshId )
{
	if( !bValidLoad  )
		return;

	unsigned int i = 0;
	math::Vec4Lite origColour;

	if( bHasNormals )
		renderer::OpenGL::GetInstance()->EnableNormalArray();

	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		if( pOpaqueMeshList[i].nMeshId != meshId )
			continue;

		if( pOpaqueMeshList && pOpaqueMeshList[i].bDraw )
		{
			if( pOpaqueMeshList[i].meshSimpleColourFlag )
			{
				origColour = renderer::OpenGL::GetInstance()->GetColour4ub();
				renderer::OpenGL::GetInstance()->SetColour4ub( pOpaqueMeshList[i].meshSimpleColour.R, pOpaqueMeshList[i].meshSimpleColour.G, pOpaqueMeshList[i].meshSimpleColour.B, pOpaqueMeshList[i].meshSimpleColour.A );
			}
			else if( m_UseDefaultColour )
				renderer::OpenGL::GetInstance()->SetColour4ub( m_DefaultMeshColour.R, m_DefaultMeshColour.G, m_DefaultMeshColour.B, m_DefaultMeshColour.A );

			// save old state
			if( pOpaqueMeshList[i].meshFogState.bFogState )
			{
				renderer::OpenGL::GetInstance()->SaveFogState( &pOpaqueMeshList[i].savedFogState );
				// set new state
				renderer::OpenGL::GetInstance()->SetFogState( &pOpaqueMeshList[i].meshFogState );
			}

			if( nGeneralFlags & MODELFLAG_GENERAL_AUTOCULLING )
			{
				if( nGeneralFlags & MODELFLAG_GENERAL_SPHERECULL )
				{
					collision::Sphere sphere = pOpaqueMeshList[i].meshSphere;

					if( nGeneralFlags & MODELFLAG_GENERAL_MODELCULL )
					{
						sphere = modelSphere;
					}
					else if( nGeneralFlags & MODELFLAG_GENERAL_SUBMESHCULL )
					{
						sphere = pOpaqueMeshList[i].meshSphere;
					}

					if( renderer::OpenGL::GetInstance()->SphereInFrustum( sphere.vCenterPoint.X, sphere.vCenterPoint.Y, sphere.vCenterPoint.Z, sphere.fRadius ) )
					{
						if( pOpaqueMeshList[i].meshTranslate || 
							pOpaqueMeshList[i].meshRotate ||
							pOpaqueMeshList[i].meshScale )
						{
							glPushMatrix();
							if( pOpaqueMeshList[i].meshTranslate )
								glTranslatef( pOpaqueMeshList[i].meshTranslation.X, pOpaqueMeshList[i].meshTranslation.Y, pOpaqueMeshList[i].meshTranslation.Z );
							if( pOpaqueMeshList[i].meshScale )
							{
								glScalef( pOpaqueMeshList[i].meshScaleFactors.X, pOpaqueMeshList[i].meshScaleFactors.Y, pOpaqueMeshList[i].meshScaleFactors.Z );
							}
							if( pOpaqueMeshList[i].meshRotate )
							{
								glRotatef( pOpaqueMeshList[i].meshRotation.X, 1.0f, 0.0f, 0.0f );
								glRotatef( pOpaqueMeshList[i].meshRotation.Y, 0.0f, 1.0f, 0.0f );
								glRotatef( pOpaqueMeshList[i].meshRotation.Z, 0.0f, 0.0f, 1.0f );
							}

						}

						// draw the mesh
						pOpaqueMeshList[i].Draw();

						if( pOpaqueMeshList[i].meshTranslate ||
							pOpaqueMeshList[i].meshRotate ||
							pOpaqueMeshList[i].meshScale )
						{
							glPopMatrix();
						}
					
						if( renderer::OpenGL::IsInitialised() )
						{
							renderer::OpenGL::GetInstance()->nTotalTriangleCount += pOpaqueMeshList[i].nTotalTriangleCount;
							renderer::OpenGL::GetInstance()->nTotalVertexCount += pOpaqueMeshList[i].nTotalVertexCount;
						}
					}
				}

				if( nGeneralFlags & MODELFLAG_GENERAL_AABBCULL )
				{
					collision::AABB aabb = pOpaqueMeshList[i].meshAABB;

					if( nGeneralFlags & MODELFLAG_GENERAL_MODELCULL )
					{
						aabb = modelAABB;
					}
					else if( nGeneralFlags & MODELFLAG_GENERAL_SUBMESHCULL )
					{
						aabb = pOpaqueMeshList[i].meshAABB;
					}

					if( renderer::OpenGL::GetInstance()->AABBInFrustum( aabb ) )
					{
						if( pOpaqueMeshList[i].meshTranslate || 
							pOpaqueMeshList[i].meshRotate ||
							pOpaqueMeshList[i].meshScale )
						{
							glPushMatrix();
							if( pOpaqueMeshList[i].meshTranslate )
								glTranslatef( pOpaqueMeshList[i].meshTranslation.X, pOpaqueMeshList[i].meshTranslation.Y, pOpaqueMeshList[i].meshTranslation.Z );
							if( pOpaqueMeshList[i].meshScale )
							{
								glScalef( pOpaqueMeshList[i].meshScaleFactors.X, pOpaqueMeshList[i].meshScaleFactors.Y, pOpaqueMeshList[i].meshScaleFactors.Z );
							}
							if( pOpaqueMeshList[i].meshRotate )
							{
								glRotatef( pOpaqueMeshList[i].meshRotation.X, 1.0f, 0.0f, 0.0f );
								glRotatef( pOpaqueMeshList[i].meshRotation.Y, 0.0f, 1.0f, 0.0f );
								glRotatef( pOpaqueMeshList[i].meshRotation.Z, 0.0f, 0.0f, 1.0f );
							}

						}

						// draw the mesh
						pOpaqueMeshList[i].Draw();

						if( pOpaqueMeshList[i].meshTranslate ||
							pOpaqueMeshList[i].meshRotate ||
							pOpaqueMeshList[i].meshScale )
						{
							glPopMatrix();
						}

						if( renderer::OpenGL::IsInitialised() )
						{
							renderer::OpenGL::GetInstance()->nTotalTriangleCount += pOpaqueMeshList[i].nTotalTriangleCount;
							renderer::OpenGL::GetInstance()->nTotalVertexCount += pOpaqueMeshList[i].nTotalVertexCount;
						}
					}
				}
			}
			else
			{
				if( pOpaqueMeshList[i].meshTranslate || 
					pOpaqueMeshList[i].meshRotate ||
					pOpaqueMeshList[i].meshScale )
				{
					glPushMatrix();
					if( pOpaqueMeshList[i].meshTranslate )
						glTranslatef( pOpaqueMeshList[i].meshTranslation.X, pOpaqueMeshList[i].meshTranslation.Y, pOpaqueMeshList[i].meshTranslation.Z );
					if( pOpaqueMeshList[i].meshScale )
					{
						glScalef( pOpaqueMeshList[i].meshScaleFactors.X, pOpaqueMeshList[i].meshScaleFactors.Y, pOpaqueMeshList[i].meshScaleFactors.Z );
					}
					if( pOpaqueMeshList[i].meshRotate )
					{
						glRotatef( pOpaqueMeshList[i].meshRotation.X, 1.0f, 0.0f, 0.0f );
						glRotatef( pOpaqueMeshList[i].meshRotation.Y, 0.0f, 1.0f, 0.0f );
						glRotatef( pOpaqueMeshList[i].meshRotation.Z, 0.0f, 0.0f, 1.0f );
					}

				}

				// draw the mesh
				pOpaqueMeshList[i].Draw();
			
				if( pOpaqueMeshList[i].meshTranslate ||
					pOpaqueMeshList[i].meshRotate ||
					pOpaqueMeshList[i].meshScale )
				{
					glPopMatrix();
				}

				if( renderer::OpenGL::IsInitialised() )
				{
					renderer::OpenGL::GetInstance()->nTotalTriangleCount += pOpaqueMeshList[i].nTotalTriangleCount;
					renderer::OpenGL::GetInstance()->nTotalVertexCount += pOpaqueMeshList[i].nTotalVertexCount;
				}
			}

			if( pOpaqueMeshList[i].meshSimpleColourFlag )
			{
				renderer::OpenGL::GetInstance()->SetColour4ub( origColour.R, origColour.G, origColour.B, origColour.A );
			}

			if( pOpaqueMeshList[i].meshFogState.bFogState )
			{
				// restore fog state
				renderer::OpenGL::GetInstance()->SetFogState( &pOpaqueMeshList[i].savedFogState );
			}
		}
	}

	if( bHasNormals )
		renderer::OpenGL::GetInstance()->DisableNormalArray();

	// disable any VBO arrays
	renderer::OpenGL::GetInstance()->DisableVBO();
}


/////////////////////////////////////////////////////
/// Method: EnableMeshDefaultColour
/// Params: None
///
/////////////////////////////////////////////////////
void ModelHGM::EnableMeshDefaultColour( unsigned int meshId )
{
	unsigned int i = 0;
	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		if( meshId == -1 ||
			pOpaqueMeshList[i].nMeshId == meshId )
		{
			pOpaqueMeshList[i].meshSimpleColourFlag = true;

			if( meshId != -1 )
				return;
		}
	}

	for( i = 0; i < nNumSortedMeshes; i++ )
	{
		if( meshId == -1 ||
			pSortedMeshList[i].nMeshId == meshId )
		{
			pSortedMeshList[i].meshSimpleColourFlag = true;

			if( meshId != -1 )
				return;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: DisableMeshDefaultColour
/// Params: None
///
/////////////////////////////////////////////////////
void ModelHGM::DisableMeshDefaultColour( unsigned int meshId )
{
	unsigned int i = 0;
	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		if( meshId == -1 ||
			pOpaqueMeshList[i].nMeshId == meshId )
		{
			pOpaqueMeshList[i].meshSimpleColourFlag = false;
			return;
		}
	}

	for( i = 0; i < nNumSortedMeshes; i++ )
	{
		if( meshId == -1 ||
			pSortedMeshList[i].nMeshId == meshId )
		{
			pSortedMeshList[i].meshSimpleColourFlag = false;
			return;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SetDefaultMeshColour
/// Params: [in]colour
///
/////////////////////////////////////////////////////
void ModelHGM::SetDefaultMeshColour( const math::Vec4Lite& colour )
{
	m_DefaultMeshColour = colour;
}

/////////////////////////////////////////////////////
/// Method: SetMeshDefaultColour
/// Params: [in]meshId, [in]state
///
/////////////////////////////////////////////////////
void ModelHGM::SetMeshDefaultColour( unsigned int meshId, const math::Vec4Lite& colour )
{
	unsigned int i = 0;
	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		if( meshId == -1 ||
			pOpaqueMeshList[i].nMeshId == meshId )
		{
			pOpaqueMeshList[i].meshSimpleColour = colour;
			return;
		}
	}

	for( i = 0; i < nNumSortedMeshes; i++ )
	{
		if( meshId == -1 ||
			pSortedMeshList[i].nMeshId == meshId )
		{
			pSortedMeshList[i].meshSimpleColour = colour;
			return;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: GetMeshDefaultColour
/// Params: [in]meshId
///
/////////////////////////////////////////////////////
math::Vec4Lite& ModelHGM::GetMeshDefaultColour( unsigned int meshId )
{
	unsigned int i = 0;
	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		if( pOpaqueMeshList[i].nMeshId == meshId )
		{
			return pOpaqueMeshList[i].meshSimpleColour;
		}
	}

	for( i = 0; i < nNumSortedMeshes; i++ )
	{
		if( pSortedMeshList[i].nMeshId == meshId )
		{
			return pSortedMeshList[i].meshSimpleColour;
		}
	}

	return defaultWhite;
}

/////////////////////////////////////////////////////
/// Method: SetMeshDrawState
/// Params: [in]meshId, [in]state
///
/////////////////////////////////////////////////////
void ModelHGM::SetMeshDrawState( int meshId, bool state )
{
	unsigned int i = 0;
	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		if( meshId == -1 ||
			pOpaqueMeshList[i].nMeshId == meshId )
		{
			pOpaqueMeshList[i].bDraw = state;
			if( meshId != -1 )
				return;
		}
	}

	for( i = 0; i < nNumSortedMeshes; i++ )
	{
		if( meshId == -1 ||
			pSortedMeshList[i].nMeshId == meshId )
		{
			pSortedMeshList[i].bDraw = state;
			if( meshId != -1 )
				return;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SetMeshTranslation
/// Params: [in]meshId, [in]trans
///
/////////////////////////////////////////////////////
void ModelHGM::SetMeshTranslation( int meshId, const math::Vec3& trans )
{
	unsigned int i = 0;
	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		if( meshId == -1 ||
			pOpaqueMeshList[i].nMeshId == meshId )
		{
			pOpaqueMeshList[i].meshTranslate = true;
			pOpaqueMeshList[i].meshTranslation = trans;
			if( meshId != -1 )
				return;
		}
	}

	for( i = 0; i < nNumSortedMeshes; i++ )
	{
		if( meshId == -1 ||
			pSortedMeshList[i].nMeshId == meshId )
		{
			pSortedMeshList[i].meshTranslate = true;
			pSortedMeshList[i].meshTranslation = trans;
			if( meshId != -1 )
				return;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SetMeshRotation
/// Params: [in]meshId, [in]rot
///
/////////////////////////////////////////////////////
void ModelHGM::SetMeshRotation( int meshId, const math::Vec3& rot )
{
	unsigned int i = 0;
	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		if( meshId == -1 ||
			pOpaqueMeshList[i].nMeshId == meshId )
		{
			pOpaqueMeshList[i].meshRotate = true;
			pOpaqueMeshList[i].meshRotation = rot;
			if( meshId != -1 )
				return;
		}
	}

	for( i = 0; i < nNumSortedMeshes; i++ )
	{
		if( meshId == -1 ||
			pSortedMeshList[i].nMeshId == meshId )
		{
			pSortedMeshList[i].meshRotate = true;
			pSortedMeshList[i].meshRotation = rot;
			if( meshId != -1 )
				return;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SetMeshScale
/// Params: [in]meshId, [in]scale
///
/////////////////////////////////////////////////////
void ModelHGM::SetMeshScale( int meshId, const math::Vec3& scale )
{
	unsigned int i = 0;
	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		if( meshId == -1 ||
			pOpaqueMeshList[i].nMeshId == meshId )
		{
			pOpaqueMeshList[i].meshScale = true;
			pOpaqueMeshList[i].meshScaleFactors = scale;
			
			if( meshId != -1 )
				return;
		}
	}

	for( i = 0; i < nNumSortedMeshes; i++ )
	{
		if( meshId == -1 ||
			pSortedMeshList[i].nMeshId == meshId )
		{
			pSortedMeshList[i].meshScale = true;
			pSortedMeshList[i].meshScaleFactors = scale;

			if( meshId != -1 )
				return;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]fDelta
///
/////////////////////////////////////////////////////
void ModelHGM::Update( float fDelta, bool meshSort )
{
	unsigned int i = 0;
	if( meshSort )
	{
		math::Vec3 vEye, vCenter;
		renderer::OpenGL::GetInstance()->GetLookAt( vEye, vCenter );

		for( i = 0; i < nNumSortedMeshes; i++ )
		{
			math::Vec3 lenVec = pSortedMeshList[i].meshAABB.vCenter - vEye;

			pSortedMeshList[i].sceneDepth = lenVec.length();
		}

		// qsort
		std::qsort( pSortedMeshList, GetNumSortedMeshes(), sizeof(Mesh), MeshSortByDepth );
	}
	else
	{
		for( i = 0; i < nNumSortedMeshes; i++ )
		{
			pSortedMeshList[i].Update( fDelta );
		}

		for( i = 0; i < nNumOpaqueMeshes; i++ )
		{
			pOpaqueMeshList[i].Update( fDelta );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void ModelHGM::Release( void )
{
	unsigned int i=0;

	for( i=0; i < m_TextureIDList.size(); ++i )
	{
		if( m_TextureIDList[i] != renderer::INVALID_OBJECT )
		{
			renderer::RemoveTexture( m_TextureIDList[i] );	
		}
	}

	m_TextureFormatList.clear();
	m_MaterialFormatList.clear();

	m_TextureIDList.clear();
	m_MaterialBlockList.clear();

	if( m_TexAnimationBlocks != 0 )
	{
		for( i=0; i < m_NumAnimationBlocks; ++i )
		{
			if( m_TexAnimationBlocks[i].animationData != 0 )
				delete[] m_TexAnimationBlocks[i].animationData;

			if( m_TexAnimationBlocks[i].pSubMeshList != 0 )
				delete[] m_TexAnimationBlocks[i].pSubMeshList;
		}

		delete[] m_TexAnimationBlocks;
	}

	// call the Model shutdown to clear other default data
	Shutdown();

	// reset any HGM data
	Initialise();
}

/////////////////////////////////////////////////////
/// Method: LoadTextureAnimation
/// Params: [in]block
///
/////////////////////////////////////////////////////
void ModelHGM::LoadTextureAnimations( unsigned int blockCount, mdl::TextureAnimationBlock* blocks )
{
	unsigned int i=0, j=0;

	m_NumAnimationBlocks = blockCount;
	m_TexAnimationBlocks = blocks;

	// for each animation
	for( i=0; i < m_NumAnimationBlocks; ++i )
	{
		// and for each submesh in the animations list
		for( j=0; j < (unsigned int)m_TexAnimationBlocks[i].numSubmeshIds; ++j )
		{
			// find the mesh
			mdl::Mesh* pMesh = GetMeshFromIndex( m_TexAnimationBlocks[i].pSubMeshList[j].meshId );
			DBG_ASSERT( pMesh != 0 );

			m_TexAnimationBlocks[i].pSubMeshList[j].pMesh = pMesh;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SetTextureAnimation
/// Params: None
///
/////////////////////////////////////////////////////
void ModelHGM::SetTextureAnimation( unsigned int animationIndex )
{
	DBG_ASSERT( (animationIndex < m_NumAnimationBlocks) );
	int i=0;

	// and for each submesh in the animations list
	for( i=0; i < m_TexAnimationBlocks[animationIndex].numSubmeshIds; ++i )
	{
		m_TexAnimationBlocks[animationIndex].pSubMeshList[i].pMesh->SetTextureAnimation( m_TexAnimationBlocks[animationIndex].numAnimationFrames, m_TexAnimationBlocks[animationIndex].animationData );
	}
}

/////////////////////////////////////////////////////
/// Method: CopyMaterials
/// Params: None
///
/////////////////////////////////////////////////////
void ModelHGM::CopyMaterials( int matIndex, int subMesh )
{
	unsigned int i = 0, j = 0;

	// go through each mesh
	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		// for each material
		for( j = 0; j < pOpaqueMeshList[i].nNumMaterialTriangleLists; j++ )
		{
			// re-copy the material information

			if (subMesh == -1 ||
				subMesh == pOpaqueMeshList[i].nMeshId)
			{
				if (pOpaqueMeshList[i].pMaterialIndexList[j] >= 0 && pOpaqueMeshList[i].pMaterialIndexList[j] < static_cast<int>(nNumMaterials))
				{
					if (matIndex == -1)
					{
						std::memcpy(&pOpaqueMeshList[i].pMaterials[j], &pMaterials[pOpaqueMeshList[i].pMaterialIndexList[j]], sizeof(TMaterialEx));
					}
					else
					{
						if (matIndex == pOpaqueMeshList[i].pMaterialIndexList[j])
							std::memcpy(&pOpaqueMeshList[i].pMaterials[j], &pMaterials[pOpaqueMeshList[i].pMaterialIndexList[j]], sizeof(TMaterialEx));
					}
				}
				else
					std::memset(&pOpaqueMeshList[i].pMaterials[j], 0, sizeof(TMaterialEx));
			}
		}
	}

	for( i = 0; i < nNumSortedMeshes; i++ )
	{
		// for each material
		for( j = 0; j < pSortedMeshList[i].nNumMaterialTriangleLists; j++ )
		{
			// re-copy the material information

			if (subMesh == -1 ||
				subMesh == pSortedMeshList[i].nMeshId)
			{
				if (pSortedMeshList[i].pMaterialIndexList[j] >= 0 && pSortedMeshList[i].pMaterialIndexList[j] < static_cast<int>(nNumMaterials))
				{
					if (matIndex == -1)
					{
						std::memcpy(&pSortedMeshList[i].pMaterials[j], &pMaterials[pSortedMeshList[i].pMaterialIndexList[j]], sizeof(TMaterialEx));
					}
					else
					{
						if (matIndex == pSortedMeshList[i].pMaterialIndexList[j])
							std::memcpy(&pSortedMeshList[i].pMaterials[j], &pMaterials[pSortedMeshList[i].pMaterialIndexList[j]], sizeof(TMaterialEx));
					}
				}
				else
					std::memset(&pSortedMeshList[i].pMaterials[j], 0, sizeof(TMaterialEx));
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SetTextureToMaterial
/// Params: [in]texID, [in]matIndex, [in]texUnit
///
/////////////////////////////////////////////////////
void ModelHGM::SetTextureToMaterial( GLuint texID, int matIndex, int texUnit, int subMesh )
{
	if( matIndex >= static_cast<int>(nNumMaterials) )
		return;

	if( texUnit >= MODEL_MAX_USEABLE_TEXUNITS )
		return;

	int i=0;

	if( matIndex == -1 )
	{
		for( i=0; i < static_cast<int>(nNumMaterials); ++i )
		{
			if( texID != renderer::INVALID_OBJECT )
				pMaterials[i].texUnit[texUnit].bEnabled = true;
			else
				pMaterials[i].texUnit[texUnit].bEnabled = false;

			pMaterials[i].texUnit[texUnit].nTextureID = texID;
		}
	}
	else
	{
		if( texID != renderer::INVALID_OBJECT )
			pMaterials[matIndex].texUnit[texUnit].bEnabled = true;
		else
			pMaterials[matIndex].texUnit[texUnit].bEnabled = false;

		pMaterials[matIndex].texUnit[texUnit].nTextureID = texID;
	}

	CopyMaterials(matIndex, subMesh);
}

/////////////////////////////////////////////////////
/// Method: GetTextureOnMaterial
/// Params: [in]matIndex, [in]texUnit
///
/////////////////////////////////////////////////////
GLuint ModelHGM::GetTextureOnMaterial( int matIndex, unsigned int texUnit )
{
	if( matIndex >= static_cast<int>(nNumMaterials) )
		return renderer::INVALID_OBJECT;

	if( texUnit >= MODEL_MAX_USEABLE_TEXUNITS )
		return renderer::INVALID_OBJECT;

	return pMaterials[matIndex].texUnit[texUnit].nTextureID;
}

/////////////////////////////////////////////////////
/// Method: SetMaterialColour
/// Params: [in]matIndex, [in]materialParam, [in]colour
///
/////////////////////////////////////////////////////
void ModelHGM::SetMaterialColour( int matIndex, GLenum materialParam, const math::Vec4& colour )
{
	if( matIndex >= static_cast<int>(nNumMaterials) )
		return;

	switch( materialParam )
	{
		case GL_DIFFUSE:
			pMaterials[matIndex].Diffuse = colour;
		break;
		case GL_AMBIENT:
			pMaterials[matIndex].Ambient = colour;
		break;
		case GL_EMISSION:
			pMaterials[matIndex].Emissive = colour;
		break;
		case GL_SPECULAR:
			pMaterials[matIndex].Specular = colour;
		break;

		default:
			break;
	}
	CopyMaterials(matIndex);
}

/////////////////////////////////////////////////////
/// Method: SetMaterialShininess
/// Params: [in]matIndex, [in]shininess
///
/////////////////////////////////////////////////////
void ModelHGM::SetMaterialShininess( int matIndex, float shininess )
{
	if( matIndex >= static_cast<int>(nNumMaterials) )
		return;

	pMaterials[matIndex].Shininess = shininess;

	CopyMaterials(matIndex);
}

/////////////////////////////////////////////////////
/// Method: SetMaterialScale
/// Params: [in]meshIndex, [in]matIndex, [in]xOffset, [in]zOffset
///
/////////////////////////////////////////////////////
void ModelHGM::SetMaterialScale( int meshIndex, int matIndex, unsigned int texUnit, float xOffset, float zOffset )
{
	unsigned int i=0, j=0;

	if( meshIndex >= static_cast<int>(nNumOpaqueMeshes) )
		return;

	if( matIndex >= static_cast<int>(nNumMaterials) )
		return;

	if( texUnit >= MODEL_MAX_USEABLE_TEXUNITS )
		return;

	if( meshIndex == -1 )
	{
		for( i = 0; i < nNumOpaqueMeshes; i++ )
		{
			for( j = 0; j < pOpaqueMeshList[i].nNumMaterialTriangleLists; j++ )
			{
				if( pOpaqueMeshList[i].pMaterialIndexList[j] == matIndex ||
					matIndex == -1 )
				{
					pOpaqueMeshList[i].pMaterials[j].texUnit[texUnit].fTextureScaleX = xOffset;
					pOpaqueMeshList[i].pMaterials[j].texUnit[texUnit].fTextureScaleZ = zOffset;
				}
			}
		}
	}
	else
	{
		for( i = 0; i < pOpaqueMeshList[meshIndex].nNumMaterialTriangleLists; i++ )
		{
			if( pOpaqueMeshList[meshIndex].pMaterialIndexList[i] == matIndex )
			{
				pOpaqueMeshList[meshIndex].pMaterials[i].texUnit[texUnit].fTextureScaleX = xOffset;
				pOpaqueMeshList[meshIndex].pMaterials[i].texUnit[texUnit].fTextureScaleZ = zOffset;
				return;
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: GetMeshFromIndex
/// Params: [in]meshIndex
///
/////////////////////////////////////////////////////
mdl::Mesh* ModelHGM::GetMeshFromIndex( unsigned int meshIndex )
{
	unsigned int i=0;

	for( i = 0; i < nNumOpaqueMeshes; i++ )
	{
		if( pOpaqueMeshList[i].nMeshIndex == meshIndex )
		{
			return &pOpaqueMeshList[i];
		}
	}

	// not found
	return 0;
}

#endif // defined(BASE_SUPPORT_OPENGL) || defined(BASE_SUPPORT_OPENGLES)
