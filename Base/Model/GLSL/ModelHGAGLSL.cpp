
/*===================================================================
	File: ModelHGAGLSL.cpp
	Library: ModelLoaderLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_SUPPORT_OPENGL_GLSL

#include "CoreBase.h"

#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"

#include "Model/ModelCommon.h"
#include "Model/Model.h"
#include "Model/GLSL/MeshGLSL.h"
#include "Model/GLSL/SkinMeshGLSL.h"

#include "Model/GLSL/ModelHGAGLSL.h"

using mdl::ModelHGA;

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
ModelHGA::ModelHGA()
{
	Initialise();
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
ModelHGA::~ModelHGA()
{
	Release();
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void ModelHGA::Initialise( void )
{
	// Model init
	Init();

	bValidLoad = false;

	nGeneralFlags			= 0;//MODELFLAG_GENERAL_AUTOCULLING;

	m_TotalPolyCount		= 0;
	m_TotalTriCount		= 0;

	m_BindPoseFileLoaded = false;
	nFileBoneOffset = 0;
	m_Paused = false;

	std::memset( &CharacterFile, 0, sizeof( file::TFile ) );
	std::memset( &ConfigFile, 0, sizeof( file::TFile ) );
	std::memset( &BindPoseFile, 0, sizeof( file::TFile ) );

	pPrimaryAnim = 0;
	pPolledAnim = 0;
	pSecondaryAnim = 0;
	pSecondaryPolledAnim = 0;

	pBoneControllers = 0;

	m_TextureFormatList.clear();
	m_MaterialFormatList.clear();

	m_TextureIDList.clear();
	m_MaterialBlockList.clear();
}

/////////////////////////////////////////////////////
/// Method: Load
/// Params: [in]szFilename
/// 
/////////////////////////////////////////////////////
int ModelHGA::Load( const char *szFilename, GLenum magFilter, GLenum minFilter, bool lowPower )
{
	return( LoadBindPose( szFilename, magFilter, minFilter, lowPower ) );
}

/////////////////////////////////////////////////////
/// Method: LoadBindPose
/// Params: [in]szFilename
///
/////////////////////////////////////////////////////
int ModelHGA::LoadBindPose( const char *szFilename, GLenum magFilter, GLenum minFilter, bool lowPower )
{
	file::TFileHandle fileHandle;
	unsigned int i = 0, j = 0, k = 0;

	char szRefFile[core::MAX_PATH];
	std::memset( szRefFile, 0, sizeof(char)*core::MAX_PATH );

	if( !core::IsEmptyString( ConfigFile.szPath ) && !core::IsEmptyString( szFilename ) )
		std::sprintf( szRefFile, "%s%s%s", ConfigFile.szDrive, ConfigFile.szPath, szFilename );

	// dodgy path, try and use the file name as is
	if( core::IsEmptyString( szRefFile ) )
	{
		std::sprintf( szRefFile, "%s", szFilename );
		
		core::SplitPath( szFilename, ConfigFile.szDrive, ConfigFile.szPath, ConfigFile.szFile, ConfigFile.szFileExt );
		core::SplitPath( szFilename, ModelFile.szDrive, ModelFile.szPath, ModelFile.szFile, ModelFile.szFileExt );
	}

	if( file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, &fileHandle ) == false )
	{
		DBGLOG( "MODELHGA: *ERROR* Could not open %s file\n", szFilename );
		return(1);
	}

	// find length of file
	nFileSize = fileHandle.nFileLength;

	std::sprintf( BindPoseFile.szDrive, "%s", ConfigFile.szDrive );

	if( !core::IsEmptyString( ConfigFile.szPath ) )
		std::sprintf( BindPoseFile.szPath, "%s", ConfigFile.szPath );

	core::SplitPath( szFilename, 0, 0, BindPoseFile.szFile, BindPoseFile.szFileExt );

	if( !core::IsEmptyString( BindPoseFile.szPath ) && !core::IsEmptyString( szFilename ) )
		std::sprintf( BindPoseFile.szFilename, "%s%s%s", BindPoseFile.szDrive, BindPoseFile.szPath, szFilename );

	if( !core::IsEmptyString( BindPoseFile.szFile ) && !core::IsEmptyString( BindPoseFile.szFileExt ) )
		std::sprintf( BindPoseFile.szFileAndExt, "%s%s", BindPoseFile.szFile, BindPoseFile.szFileExt );

	unsigned int type=0;
	file::FileReadUInt( &type, sizeof(unsigned int), 1, &fileHandle );

	file::FileReadInt( &bHasVertices, sizeof(int), 1, &fileHandle );
	file::FileReadInt( &bHasTextureCoords, sizeof(int), 1, &fileHandle );
	file::FileReadInt( &bHasNormals, sizeof(int), 1, &fileHandle );

	bool bHasTangents = false;

	if( type == 0 ) // bind pose
	{
		math::Vec3		*pNormalList = 0;
		math::Vec2		*pTexCoordList = 0;

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
					file::TFile hgaPath;
					char texturePath[core::MAX_PATH+core::MAX_PATH];
					char texFile[core::MAX_PATH];
					std::memset( &texFile, 0, sizeof(char)*core::MAX_PATH );
					std::memset( &texturePath, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );

					file::FileReadTerminatedString( texFile, core::MAX_PATH-1, &fileHandle );
					
					// build the textures path
					file::CreateFileStructure( szFilename, &hgaPath );

					renderer::ETextureFormat texFormat = renderer::TEXTURE_UNKNOWN;
#if defined(BASE_PLATFORM_WINDOWS) || defined(BASE_PLATFORM_ANDROID) || defined(BASE_PLATFORM_RASPBERRYPI) || defined(BASE_PLATFORM_MAC)
					// test tga
					snprintf( texturePath, core::MAX_PATH+core::MAX_PATH, "%s%stextures/%s.tga", hgaPath.szDrive, hgaPath.szPath, texFile );
					texFormat = renderer::TEXTURE_TGA;

					// test png
					if( !file::FileExists(texturePath) )
					{
						snprintf( texturePath, core::MAX_PATH+core::MAX_PATH, "%s%stextures/%s.png", hgaPath.szDrive, hgaPath.szPath, texFile );
						texFormat = renderer::TEXTURE_PNG;
					}
#else
					if( lowPower )
					{
						// test pvr
						snprintf( texturePath, core::MAX_PATH+core::MAX_PATH, "%s%stextures/%s.pvr", hgaPath.szDrive, hgaPath.szPath, texFile );
						texFormat = renderer::TEXTURE_PVR;

						// test png
						if( !file::FileExists(texturePath) )
						{
							snprintf( texturePath, core::MAX_PATH+core::MAX_PATH, "%s%stextures/%s.png", hgaPath.szDrive, hgaPath.szPath, texFile );
							texFormat = renderer::TEXTURE_PNG;
						}
	
					}
					else
					{
						snprintf( texturePath, core::MAX_PATH+core::MAX_PATH, "%s%stextures/%s.png", hgaPath.szDrive, hgaPath.szPath, texFile );
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
					if( materialBlock.lambertBlock.colourIdx == -1 )
						pMaterials[i].Diffuse = math::Vec4( materialBlock.lambertBlock.colour.R*materialBlock.lambertBlock.diffuseCoeff, materialBlock.lambertBlock.colour.G*materialBlock.lambertBlock.diffuseCoeff, materialBlock.lambertBlock.colour.B*materialBlock.lambertBlock.diffuseCoeff, 1.0f );
					else
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
						bHasTangents = true;

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
					if( materialBlock.blinnBlock.lambert.colourIdx == -1 )
						pMaterials[i].Diffuse = math::Vec4( materialBlock.blinnBlock.lambert.colour.R*materialBlock.blinnBlock.lambert.diffuseCoeff, materialBlock.blinnBlock.lambert.colour.G*materialBlock.blinnBlock.lambert.diffuseCoeff, materialBlock.blinnBlock.lambert.colour.B*materialBlock.blinnBlock.lambert.diffuseCoeff, 1.0f );
					else
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
						bHasTangents = true;

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
					if( materialBlock.phongBlock.lambert.colourIdx == -1 )
						pMaterials[i].Diffuse = math::Vec4( materialBlock.phongBlock.lambert.colour.R*materialBlock.phongBlock.lambert.diffuseCoeff, materialBlock.phongBlock.lambert.colour.G*materialBlock.phongBlock.lambert.diffuseCoeff, materialBlock.phongBlock.lambert.colour.B*materialBlock.phongBlock.lambert.diffuseCoeff, 1.0f );
					else
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
						bHasTangents = true;

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

		nFileBoneOffset = static_cast<long>( file::FilePosition(&fileHandle) );

		// skin counts
		file::FileReadUInt( &nNumSkins, sizeof(unsigned int), 1, &fileHandle );

		// skeleton
		file::FileReadUInt( &nNumBoneJoints, sizeof(unsigned int), 1, &fileHandle );

		DBG_MEMTRY
			pBoneJoints = new TBoneJoint[nNumBoneJoints];
		DBG_MEMCATCH

		for( i = 0; i < nNumBoneJoints; i++ )
		{
			file::FileReadInt( &pBoneJoints[i].nJointIndex, sizeof(int), 1, &fileHandle );
			file::FileReadInt( &pBoneJoints[i].nParentJointIndex, sizeof(int), 1, &fileHandle );
		}

		// allocate bone controller data
		DBG_MEMTRY
			pBoneControllers = new TBoneJointController[nNumBoneJoints];
		DBG_MEMCATCH

		for( i = 0; i < nNumBoneJoints; i++ )
		{
			pBoneControllers[i].nBoneIndex = -1;
			pBoneControllers[i].qRotation = math::Quaternion( 0.0f, 0.0f, 0.0f, 1.0f );
		}

		// bind pose
		for( i = 0; i < nNumBoneJoints; i++ )
		{
			int nJointID = 0;
			math::Vec3 vPos;
			math::Vec4 vRot;
			math::Vec3 vScale;
			math::Quaternion qRotOri;
			math::Quaternion qJointOri;

			file::FileReadInt( &nJointID, sizeof(int), 1, &fileHandle );
			file::FileReadVec3D( &vPos, sizeof(math::Vec3), 1, &fileHandle );
			file::FileReadVec4D( &vRot, sizeof(math::Vec4), 1, &fileHandle );
			file::FileReadVec3D( &vScale, sizeof(math::Vec3), 1, &fileHandle );

			pBoneJoints[nJointID].vTranslate.X = vPos.X;
			pBoneJoints[nJointID].vTranslate.Y = vPos.Y;
			pBoneJoints[nJointID].vTranslate.Z = vPos.Z;

			// because maya uses a different default setup, just negate each value to get the correct rotation/orientation
			pBoneJoints[nJointID].qRotation.X = vRot.X;
			pBoneJoints[nJointID].qRotation.Y = vRot.Y;
			pBoneJoints[nJointID].qRotation.Z = vRot.Z;
			pBoneJoints[nJointID].qRotation.W = vRot.W;

			pBoneJoints[nJointID].vScale.X = vScale.X;
			pBoneJoints[nJointID].vScale.Y = vScale.Y;
			pBoneJoints[nJointID].vScale.Z = vScale.Z;
		}

		// skin mesh lists
		DBG_MEMTRY
			pSkinMeshList = new SkinMesh[nNumSkins];
		DBG_MEMCATCH

		for( i = 0; i < nNumSkins; i++ )
		{
			// check each skin knows the data it can grab
			pSkinMeshList[i].nMeshId = i;
			pSkinMeshList[i].bHasVertices		= bHasVertices;
			pSkinMeshList[i].bHasTextureCoords	= bHasTextureCoords;
			pSkinMeshList[i].bHasNormals		= bHasNormals;
			pSkinMeshList[i].bHasTangents		= bHasTangents;

			file::FileReadInt( &pSkinMeshList[i].nNumPoints, sizeof(int), 1, &fileHandle );
			file::FileReadInt( &pSkinMeshList[i].nNumTextureCoords, sizeof(int), 1, &fileHandle );
			file::FileReadInt( &pSkinMeshList[i].nNumNormals, sizeof(int), 1, &fileHandle );
			file::FileReadInt( &pSkinMeshList[i].nNumVertexColours, sizeof(int), 1, &fileHandle );

			// points
			if( bHasVertices )
			{
				DBG_MEMTRY
					pSkinMeshList[i].pPoints = new math::Vec3[pSkinMeshList[i].nNumPoints];
				DBG_MEMCATCH

				DBG_MEMTRY
					pSkinMeshList[i].pVertexWeights = new TVertexWeights*[ pSkinMeshList[i].nNumPoints ];
				DBG_MEMCATCH

				for( j = 0; j < pSkinMeshList[i].nNumPoints; j++ )
				{
					DBG_MEMTRY
						pSkinMeshList[i].pVertexWeights[j] = new TVertexWeights[ MAX_WEIGHTS_PER_VERTEX ];
					DBG_MEMCATCH
				}

				for( j = 0; j < pSkinMeshList[i].nNumPoints; j++ )
				{
					file::FileReadVec3D( &pSkinMeshList[i].pPoints[j], sizeof(math::Vec3), 1, &fileHandle );

					file::FileReadInt( &pSkinMeshList[i].pVertexWeights[j][0].nBoneIndex, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pSkinMeshList[i].pVertexWeights[j][0].fWeight, sizeof(float), 1, &fileHandle );

					file::FileReadInt( &pSkinMeshList[i].pVertexWeights[j][1].nBoneIndex, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pSkinMeshList[i].pVertexWeights[j][1].fWeight, sizeof(float), 1, &fileHandle );

					file::FileReadInt( &pSkinMeshList[i].pVertexWeights[j][2].nBoneIndex, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pSkinMeshList[i].pVertexWeights[j][2].fWeight, sizeof(float), 1, &fileHandle );

					file::FileReadInt( &pSkinMeshList[i].pVertexWeights[j][3].nBoneIndex, sizeof(int), 1, &fileHandle );
					file::FileReadFloat( &pSkinMeshList[i].pVertexWeights[j][3].fWeight, sizeof(float), 1, &fileHandle );
				}
			}

			// uvs
			if( bHasTextureCoords )
			{
				DBG_MEMTRY
					pTexCoordList = new math::Vec2[pSkinMeshList[i].nNumTextureCoords];
				DBG_MEMCATCH

				for( j = 0; j < pSkinMeshList[i].nNumTextureCoords; j++ )
				{
					file::FileReadVec2D( &pTexCoordList[j], sizeof(math::Vec2), 1, &fileHandle );
				}
			}

			// normals
			if( bHasNormals )
			{
				DBG_MEMTRY
					pSkinMeshList[i].pPerPointNormals = new math::Vec3[pSkinMeshList[i].nNumPoints];
				DBG_MEMCATCH

				DBG_MEMTRY
					pNormalList = new math::Vec3[pSkinMeshList[i].nNumNormals];
				DBG_MEMCATCH

				for( j = 0; j < pSkinMeshList[i].nNumNormals; j++ )
				{
					file::FileReadVec3D( &pNormalList[j], sizeof(math::Vec3), 1, &fileHandle );
				}
			}

			// per material triangle list
			file::FileReadInt( &pSkinMeshList[i].nNumMaterialTriangleLists, sizeof(int), 1, &fileHandle );

			DBG_MEMTRY
				pSkinMeshList[i].pMaterials = new TMaterialEx[ pSkinMeshList[i].nNumMaterialTriangleLists ];
			DBG_MEMCATCH

			DBG_MEMTRY
				pSkinMeshList[i].pMaterialIndexList = new int[ pSkinMeshList[i].nNumMaterialTriangleLists ];
			DBG_MEMCATCH

			DBG_MEMTRY
				pSkinMeshList[i].pTriPerMaterialList = new unsigned int[ pSkinMeshList[i].nNumMaterialTriangleLists ];
			DBG_MEMCATCH

			for( j = 0; j < pSkinMeshList[i].nNumMaterialTriangleLists; j++ )
			{
				file::FileReadInt( &pSkinMeshList[i].pMaterialIndexList[j], sizeof(int), 1, &fileHandle );
				file::FileReadInt( &pSkinMeshList[i].pTriPerMaterialList[j], sizeof(int), 1, &fileHandle );

				if( pSkinMeshList[i].pMaterialIndexList[j] >= 0 && pSkinMeshList[i].pMaterialIndexList[j] < static_cast<int>(nNumMaterials) )
					std::memcpy( &pSkinMeshList[i].pMaterials[j], &pMaterials[ pSkinMeshList[i].pMaterialIndexList[j] ], sizeof( TMaterialEx ) );
				else
					std::memset( &pSkinMeshList[i].pMaterials[j], 0, sizeof( TMaterialEx ) );
			}

			file::FileReadInt( &pSkinMeshList[i].nTotalPolyCount, sizeof(int), 1, &fileHandle );
			file::FileReadInt( &pSkinMeshList[i].nNumVertices, sizeof(int), 1, &fileHandle );
			file::FileReadInt( &pSkinMeshList[i].nTotalTriangleCount, sizeof(int), 1, &fileHandle );

			// add this to the inherited Model values
			nNumVertices += pSkinMeshList[i].nNumVertices;
			m_TotalPolyCount += pSkinMeshList[i].nTotalPolyCount;
			m_TotalTriCount += pSkinMeshList[i].nTotalTriangleCount;

			pSkinMeshList[i].nTotalVertexCount = pSkinMeshList[i].nNumVertices;

			// triangle list per material
			//verts
			if( bHasVertices )
			{
				DBG_MEMTRY
					pSkinMeshList[i].pVertices = new math::Vec3[ pSkinMeshList[i].nNumPoints ];
				DBG_MEMCATCH

				DBG_MEMTRY
					pSkinMeshList[i].pFullVertexList = new math::Vec3[ pSkinMeshList[i].nTotalVertexCount ];
				DBG_MEMCATCH

				DBG_MEMTRY
					pSkinMeshList[i].ppVertexPositionPointer = new math::Vec3*[ pSkinMeshList[i].nTotalVertexCount ];
				DBG_MEMCATCH

			}

			// tex coords
			if( bHasTextureCoords )
			{
				DBG_MEMTRY
					pSkinMeshList[i].pTexCoords = new math::Vec2[ pSkinMeshList[i].nTotalVertexCount ];
				DBG_MEMCATCH
			}

			// normals
			if( bHasNormals )
			{
				DBG_MEMTRY
					pSkinMeshList[i].pFullNormalList = new math::Vec3[ pSkinMeshList[i].nTotalVertexCount ];
				DBG_MEMCATCH

				DBG_MEMTRY
					pSkinMeshList[i].ppNormalPositionPointer = new math::Vec3*[ pSkinMeshList[i].nTotalVertexCount ];
				DBG_MEMCATCH
			}

			int nIndex = 0;
			for( j = 0; j < pSkinMeshList[i].nNumMaterialTriangleLists; j++ )
			{				
				for( k = 0; k < pSkinMeshList[i].pTriPerMaterialList[j]; k++ )
				{
					int v1, v2, v3;
					int uv1, uv2, uv3;
					int normal1, normal2, normal3;
					math::Vec4Lite colour1, colour2, colour3;
				
					if( bHasVertices )
					{
						file::FileReadInt( &v1, sizeof(int), 1, &fileHandle );
						file::FileReadInt( &v2, sizeof(int), 1, &fileHandle );
						file::FileReadInt( &v3, sizeof(int), 1, &fileHandle );
									
						pSkinMeshList[i].pVertices[v1] = pSkinMeshList[i].pPoints[v1];
						pSkinMeshList[i].pVertices[v2] = pSkinMeshList[i].pPoints[v2];
						pSkinMeshList[i].pVertices[v3] = pSkinMeshList[i].pPoints[v3];

						pSkinMeshList[i].pFullVertexList[nIndex] = pSkinMeshList[i].pVertices[v1];
						pSkinMeshList[i].pFullVertexList[nIndex+1] = pSkinMeshList[i].pVertices[v2];
						pSkinMeshList[i].pFullVertexList[nIndex+2] = pSkinMeshList[i].pVertices[v3];

						pSkinMeshList[i].ppVertexPositionPointer[nIndex] = &(pSkinMeshList[i].pVertices[v1]);
						pSkinMeshList[i].ppVertexPositionPointer[nIndex+1] = &(pSkinMeshList[i].pVertices[v2]);
						pSkinMeshList[i].ppVertexPositionPointer[nIndex+2] = &(pSkinMeshList[i].pVertices[v3]);	
					}

					if( bHasTextureCoords )
					{
						file::FileReadInt( &uv1, sizeof(int), 1, &fileHandle );
						file::FileReadInt( &uv2, sizeof(int), 1, &fileHandle );
						file::FileReadInt( &uv3, sizeof(int), 1, &fileHandle );

						pSkinMeshList[i].pTexCoords[nIndex] = pTexCoordList[uv1];
						pSkinMeshList[i].pTexCoords[nIndex+1] = pTexCoordList[uv2];
						pSkinMeshList[i].pTexCoords[nIndex+2] = pTexCoordList[uv3];
					}

					if( bHasNormals )
					{
						file::FileReadInt( &normal1, sizeof(int), 1, &fileHandle );
						file::FileReadInt( &normal2, sizeof(int), 1, &fileHandle );
						file::FileReadInt( &normal3, sizeof(int), 1, &fileHandle );

						pSkinMeshList[i].pPerPointNormals[v1] = pNormalList[normal1];
						pSkinMeshList[i].pPerPointNormals[v2] = pNormalList[normal2];
						pSkinMeshList[i].pPerPointNormals[v3] = pNormalList[normal3];

						pSkinMeshList[i].pFullNormalList[nIndex] = pSkinMeshList[i].pPerPointNormals[v1];
						pSkinMeshList[i].pFullNormalList[nIndex+1] = pSkinMeshList[i].pPerPointNormals[v2];
						pSkinMeshList[i].pFullNormalList[nIndex+2] = pSkinMeshList[i].pPerPointNormals[v3];

						pSkinMeshList[i].ppNormalPositionPointer[nIndex] = &(pSkinMeshList[i].pPerPointNormals[v1]);
						pSkinMeshList[i].ppNormalPositionPointer[nIndex+1] = &(pSkinMeshList[i].pPerPointNormals[v2]);
						pSkinMeshList[i].ppNormalPositionPointer[nIndex+2] = &(pSkinMeshList[i].pPerPointNormals[v3]);
					}

					nIndex += 3;
				}

				// calculate bounding box and sphere 
				//pSkinMeshList[i].CalculateBounds();

				float fNewRadius = 0.0f;

				// check the sub mesh box against the main mesh box
				if( pSkinMeshList[i].meshAABB.vBoxMin.X < modelAABB.vBoxMin.X )
					modelAABB.vBoxMin.X= pSkinMeshList[i].meshAABB.vBoxMin.X ;

				if( pSkinMeshList[i].meshAABB.vBoxMin.Y < modelAABB.vBoxMin.Y )
					modelAABB.vBoxMin.Y = pSkinMeshList[i].meshAABB.vBoxMin.Y;

				if( pSkinMeshList[i].meshAABB.vBoxMin.Z < modelAABB.vBoxMin.Z )
					modelAABB.vBoxMin.Z = pSkinMeshList[i].meshAABB.vBoxMin.Z;


				if( pSkinMeshList[i].meshAABB.vBoxMax.X > modelAABB.vBoxMax.X )
					modelAABB.vBoxMax.X  = pSkinMeshList[i].meshAABB.vBoxMax.X;

				if( pSkinMeshList[i].meshAABB.vBoxMax.Y > modelAABB.vBoxMax.Y )
					modelAABB.vBoxMax.Y = pSkinMeshList[i].meshAABB.vBoxMax.Y;

				if( pSkinMeshList[i].meshAABB.vBoxMax.Z > modelAABB.vBoxMax.Z )
					modelAABB.vBoxMax.Z = pSkinMeshList[i].meshAABB.vBoxMax.Z;

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

				pSkinMeshList[i].bValidLoad = true;
				pSkinMeshList[i].bDraw = true;
			}

			nIndex = 0;
			if( pSkinMeshList[i].bHasTangents )
			{
				// Lengyel, Eric. “Computing Tangent Space Basis Vectors for an Arbitrary Mesh”. Terathon Software 3D Graphics Library, 2001. http://www.terathon.com/code/tangent.html

				int vertexCount = pSkinMeshList[i].nNumVertices;

				math::Vec3 *tan1 = new math::Vec3[vertexCount * 2];
				math::Vec3 *tan2 = tan1 + vertexCount;
				std::memset(tan1, 0, vertexCount * sizeof(math::Vec3) * 2);
    
				for (unsigned int a = 0; a < pSkinMeshList[i].nTotalTriangleCount; a++)
				{
					const math::Vec3 v1 = pSkinMeshList[i].pFullVertexList[nIndex];
					const math::Vec3 v2 = pSkinMeshList[i].pFullVertexList[nIndex+1];
					const math::Vec3 v3 = pSkinMeshList[i].pFullVertexList[nIndex+2];
        
					const math::Vec2 w1 = pSkinMeshList[i].pTexCoords[nIndex];
					const math::Vec2 w2 = pSkinMeshList[i].pTexCoords[nIndex+1];
					const math::Vec2 w3 = pSkinMeshList[i].pTexCoords[nIndex+2];
        
					float x1 = v2.X - v1.X;
					float x2 = v3.X - v1.X;
					float y1 = v2.Y - v1.Y;
					float y2 = v3.Y - v1.Y;
					float z1 = v2.Z - v1.Z;
					float z2 = v3.Z - v1.Z;
        
					float s1 = w2.X - w1.X;
					float s2 = w3.X - w1.X;
					float t1 = w2.Y - w1.Y;
					float t2 = w3.Y - w1.Y;
        
					float r = 1.0F / (s1 * t2 - s2 * t1);
					math::Vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
								(t2 * z1 - t1 * z2) * r);
					math::Vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
								(s1 * z2 - s2 * z1) * r);
        
					tan1[nIndex] += sdir;
					tan1[nIndex+1] += sdir;
					tan1[nIndex+2] += sdir;
        
					tan2[nIndex] += tdir;
					tan2[nIndex+1] += tdir;
					tan2[nIndex+2] += tdir;

					// next tri
					nIndex += 3;
				}

				// allocate tangents
				DBG_MEMTRY
					pSkinMeshList[i].pTangents = new math::Vec4[ pSkinMeshList[i].nTotalVertexCount ];
				DBG_MEMCATCH
    
				for (long a = 0; a < vertexCount; a++)
				{
					const math::Vec3 n = pSkinMeshList[i].pFullNormalList[a];
					const math::Vec3& t = tan1[a];
        
					// Gram-Schmidt orthogonalize
					math::Vec3 tGS = (t - n * math::DotProduct(n, t)).normalise();
					pSkinMeshList[i].pTangents[a].X = tGS[0];
					pSkinMeshList[i].pTangents[a].Y = tGS[1];
					pSkinMeshList[i].pTangents[a].Z = tGS[2];
        
					// Calculate handedness
					pSkinMeshList[i].pTangents[a].W = (math::DotProduct(math::CrossProduct(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
				}
    
				delete[] tan1;
			}

			if( pTexCoordList )
			{
				delete[] pTexCoordList;
				pTexCoordList = 0;
			}
			if( pNormalList )
			{
				delete[] pNormalList;
				pNormalList = 0;
			}
		}
	}

	// close the file
	file::FileClose( &fileHandle );

	bValidLoad = true;

	SetupBindPose();

	return(0);
}

/////////////////////////////////////////////////////
/// Method: SetupAndFree
/// Params: None
/// 
/////////////////////////////////////////////////////
void ModelHGA::SetupAndFree()
{
	if( !bValidLoad  )
		return;

	if( core::app::GetUseVertexArrays() )
		return;

	unsigned int i = 0;
	for( i = 0; i < nNumSkins; i++ )
	{
		if( pSkinMeshList )
		{
			// automatically frees the local memory after upload
			pSkinMeshList[i].SetupVertexBufferObject();
		}
	}
}

/////////////////////////////////////////////////////
/// Method: LoadAnimation
/// Params: [in]szFilename, [in]nAnimId, [in]nLoopFlag
///
/////////////////////////////////////////////////////
int ModelHGA::LoadAnimation( const char *szFilename, unsigned int nAnimId, bool bLoopFlag )
{
	file::TFileHandle fileHandle;
	unsigned int i = 0, j = 0;
	unsigned int nJointCount = 0;
	unsigned int nFrameCount = 0;

	if( core::IsEmptyString( szFilename ) )
		return(1);

	if( file::FileOpen( szFilename, file::FILETYPE_BINARY_READ, &fileHandle ) == false )
	{
		DBGLOG( "MODELHGA: *ERROR* Could not open %s file\n", szFilename );
		return(1);
	}

	unsigned int type=0;
	file::FileReadUInt( &type, sizeof(unsigned int), 1, &fileHandle );

	if( type == 1 )
	{
		// check joint count is the same as the reference file
		file::FileReadUInt( &nJointCount, sizeof(unsigned int), 1, &fileHandle );

		if( nJointCount != nNumBoneJoints )
		{
			DBGLOG( "MODELHGA: *ERROR* Animation file does not have the same number of bone joints as the loaded skeleton\n" );
			return(1);
		}

		// read in the animation
			file::FileReadUInt( &nFrameCount, sizeof(unsigned int), 1, &fileHandle );		

			BoneAnimation *pNewAnim = 0;
			DBG_MEMTRY
				pNewAnim = new BoneAnimation;
			DBG_MEMCATCH

			file::FileReadFloat( &pNewAnim->fAnimationFramerate, sizeof(float), 1, &fileHandle );

			if( !core::IsEmptyString( szFilename ) )
			{
				std::sprintf( pNewAnim->AnimationFile.szDrive, "%s", ConfigFile.szDrive );
				std::sprintf( pNewAnim->AnimationFile.szPath, "%s", ConfigFile.szPath );

				core::SplitPath( szFilename, 0, 0, pNewAnim->AnimationFile.szFile, pNewAnim->AnimationFile.szFileExt );
				std::sprintf( pNewAnim->AnimationFile.szFileAndExt, "%s%s", pNewAnim->AnimationFile.szFile, pNewAnim->AnimationFile.szFileExt );
				
				std::sprintf( pNewAnim->AnimationFile.szFilename, "%s%s%s", pNewAnim->AnimationFile.szDrive, pNewAnim->AnimationFile.szPath, pNewAnim->AnimationFile.szFileAndExt );
			}

			pNewAnim->nAnimId = nAnimId;
			pNewAnim->bLooping = bLoopFlag;
			pNewAnim->nTotalJoints = nJointCount;
			pNewAnim->nTotalFrames = nFrameCount;
	
			// allocate joint frame memory
			DBG_MEMTRY
				pNewAnim->pFrames = new TBoneJointKeyframe*[nJointCount]; 
			DBG_MEMCATCH

			pNewAnim->dTotalAnimationTime = pNewAnim->nTotalFrames * 1000.0f / pNewAnim->fAnimationFramerate;
	
			// for each joint allocate the number of animation frames
			for( i = 0; i < nJointCount; i++ )
			{
				DBG_MEMTRY
					pNewAnim->pFrames[i] = new TBoneJointKeyframe[nFrameCount]; 
				DBG_MEMCATCH
			}

			// now go through each frame
			int nFrameIndex = 0;
			for( i = 0; i < nFrameCount; i++ )
			{
				file::FileReadUInt( &nFrameIndex, sizeof(unsigned int), 1, &fileHandle );

				// make sure both times match
				if( nFrameIndex != i )
					break;

				// now go through each joint and add the frame data
				for( j = 0; j < nJointCount; j++ )
				{
					int nJointIndex = 0;
					math::Vec3 vPos;
					math::Vec4 vRot;
					math::Vec3 vScale;


					file::FileReadInt( &nJointIndex, sizeof(int), 1, &fileHandle );
					file::FileReadVec3D( &vPos, sizeof(math::Vec3), 1, &fileHandle );
					file::FileReadVec4D( &vRot, sizeof(math::Vec4), 1, &fileHandle );
					file::FileReadVec3D( &vScale, sizeof(math::Vec3), 1, &fileHandle );

					// time
					pNewAnim->pFrames[j][nFrameIndex].fTime = nFrameIndex * 1000.0f / pNewAnim->fAnimationFramerate;
						
					pNewAnim->pFrames[j][nFrameIndex].vTranslate.X = vPos.X;
					pNewAnim->pFrames[j][nFrameIndex].vTranslate.Y = vPos.Y;
					pNewAnim->pFrames[j][nFrameIndex].vTranslate.Z = vPos.Z;

					// because maya uses a different default setup, just negate each value to get the correct rotation/orientation
					pNewAnim->pFrames[j][nFrameIndex].qRotation.X = vRot.X;
					pNewAnim->pFrames[j][nFrameIndex].qRotation.Y = vRot.Y;
					pNewAnim->pFrames[j][nFrameIndex].qRotation.Z = vRot.Z;
					pNewAnim->pFrames[j][nFrameIndex].qRotation.W = vRot.W;

					pNewAnim->pFrames[j][nFrameIndex].vScale.X = vScale.X;
					pNewAnim->pFrames[j][nFrameIndex].vScale.Y = vScale.Y;
					pNewAnim->pFrames[j][nFrameIndex].vScale.Z = vScale.Z;

				}
			}
		
			// check filename and anim name is ok
			int nNameClash = false;
			for( i=0; i < BoneAnimationList.size(); ++i )
			{
				if( BoneAnimationList[i]->nAnimId == pNewAnim->nAnimId ||
					!std::strcmp( BoneAnimationList[i]->AnimationFile.szFileAndExt,  pNewAnim->AnimationFile.szFileAndExt ) )
				{
					// name clash
					nNameClash = true;
					DBGLOG( "MODELHGA: Config file has the same animation id or filename listed more than once\n" );
				}
			}

			if( !nNameClash )
			{
				// add to list
				BoneAnimationList.push_back( pNewAnim );
			}
			else
			{
				// delete the created anim
				if( pNewAnim )
				{
					delete pNewAnim;
					pNewAnim = 0;
				}
			}
	}

	// close the file
	file::FileClose( &fileHandle );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void ModelHGA::Draw( void )
{
	unsigned int i = 0;

	// model sphere not in the frustum ?
	if( nGeneralFlags & MODELFLAG_GENERAL_AUTOCULLING )
		if( !renderer::OpenGL::GetInstance()->SphereInFrustum( modelSphere.vCenterPoint.X, modelSphere.vCenterPoint.Y, modelSphere.vCenterPoint.Z, modelSphere.fRadius ) )
			return;

	if( nActiveProgram == renderer::INVALID_OBJECT ||
		nActiveProgram == 0 )
		return;

	// grab all matrices
	glm::mat4 projMatrix	= renderer::OpenGL::GetInstance()->GetProjectionMatrix();
	glm::mat4 viewMatrix	= renderer::OpenGL::GetInstance()->GetViewMatrix();
	glm::mat4 identity(1.0f);

	renderer::OpenGL::GetInstance()->UseProgram(nActiveProgram);

	for( i = 0; i < nNumSkins; i++ )
	{	
		glm::mat4 modelMatrix	= renderer::OpenGL::GetInstance()->GetModelMatrix();

		if( pSkinMeshList[i].bDraw )
		{
			glm::mat4 modelViewMatrix = viewMatrix*modelMatrix;

			// gl_ModelViewProjectionMatrix
			GLint ogl_ModelViewProjectionMatrix = glGetUniformLocation(nActiveProgram, "ogl_ModelViewProjectionMatrix");
			if( ogl_ModelViewProjectionMatrix != -1 )
				glUniformMatrix4fv( ogl_ModelViewProjectionMatrix, 1, GL_FALSE, glm::value_ptr(projMatrix*modelViewMatrix) );
	
			GLint ogl_ModelViewMatrix = glGetUniformLocation(nActiveProgram, "ogl_ModelViewMatrix");
			if( ogl_ModelViewMatrix != -1 )
				glUniformMatrix4fv( ogl_ModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelViewMatrix) );

			GLint ogl_ViewMatrix = glGetUniformLocation(nActiveProgram, "ogl_ViewMatrix");
			if( ogl_ViewMatrix != -1 )
				glUniformMatrix4fv( ogl_ViewMatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix) );

			// gl_NormalMatrix
			GLint ogl_NormalMatrix = glGetUniformLocation(nActiveProgram, "ogl_NormalMatrix"); 
			if( ogl_NormalMatrix != -1 )
			{
				glm::mat3 ogl_mNormalMatrix = glm::inverseTranspose( glm::mat3(modelViewMatrix) );
				glUniformMatrix3fv( ogl_NormalMatrix, 1, GL_FALSE, glm::value_ptr(ogl_mNormalMatrix));
			}

			pSkinMeshList[i].Draw();

			if( renderer::OpenGL::IsInitialised() )
			{
				renderer::OpenGL::GetInstance()->nTotalTriangleCount += pSkinMeshList[i].nTotalTriangleCount;
				renderer::OpenGL::GetInstance()->nTotalVertexCount += pSkinMeshList[i].nTotalVertexCount;
			}
		}
	}

	// disable any VBO arrays
	renderer::OpenGL::GetInstance()->DisableVBO();
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]fDelta
///
/////////////////////////////////////////////////////
void ModelHGA::Update( float fDelta )
{
	unsigned int i = 0, j = 0;
	TBoneJointKeyframe curFrame;
	TBoneJointKeyframe prevFrame;	

	math::Vec3 vPrimaryTranslate( 0.0f, 0.0f, 0.0f );
	math::Quaternion qPrimaryRotation( 0.0f, 0.0f, 0.0f, 1.0f );

	math::Vec3 vSecondaryTranslate( 0.0f, 0.0f, 0.0f );
	math::Quaternion qSecondaryRotation( 0.0f, 0.0f, 0.0f, 1.0f );

	math::Vec3 vCurPos( 0.0f, 0.0f, 0.0f );
	math::Vec3 vPrevPos( 0.0f, 0.0f, 0.0f );

	math::Quaternion qPrev( 0.0f, 0.0f, 0.0f, 1.0f );
	math::Quaternion qCur( 0.0f, 0.0f, 0.0f, 1.0f );

	if( pPrimaryAnim )
	{
		int nFrame = 0;
		int nStartFrame = 0;

		pPrimaryAnim->animTimer += (fDelta*1000.0f);

		// find out current time from the animation timer
		pPrimaryAnim->dCurrentTime = pPrimaryAnim->animTimer; //pPrimaryAnim->animTimer.GetTimeInMilliseconds();

		// should the current time be offset from the start frame (anims usually start at frame 0 though)
		float fTimeOffset = (float)((( nStartFrame )*1000) / pPrimaryAnim->fAnimationFramerate );
		pPrimaryAnim->dCurrentTime += fTimeOffset; 

		if( (pPrimaryAnim->nTotalFrames > 0) && (pPrimaryAnim->nTotalFrames < pPrimaryAnim->nTotalFrames) )
			pPrimaryAnim->fEndTime = (float)( ( (pPrimaryAnim->nTotalFrames)*1000) / pPrimaryAnim->fAnimationFramerate );
		else 
			pPrimaryAnim->fEndTime = (float)pPrimaryAnim->dTotalAnimationTime;

		// get the animation status
		if( pPrimaryAnim->dCurrentTime > pPrimaryAnim->fEndTime )
		{
			// animation has finished, check for a polled animation
			if( pPolledAnim )
			{
				pPrimaryAnim = pPolledAnim;

				if( pPrimaryAnim->bLooping )
				{
					pPrimaryAnim->nCurrentFrame = 0;
					//pPrimaryAnim->animTimer.Reset();
					pPrimaryAnim->animTimer = 0.0f;
					pPrimaryAnim->dCurrentTime = fTimeOffset;
				}
				else
				{
					pPrimaryAnim->dCurrentTime = pPrimaryAnim->fEndTime; 
				}

				// clear out the poll animation
				pPolledAnim = 0;
			}
			else
			{
				// no animation waiting, either loop or stop
				if( !m_Paused )
				{
					if( pPrimaryAnim->bLooping )
					{
						pPrimaryAnim->nCurrentFrame = 0;
						//pPrimaryAnim->animTimer.Reset();
						pPrimaryAnim->animTimer = 0.0f;
						pPrimaryAnim->dCurrentTime = fTimeOffset;
					}
					else
					{
						pPrimaryAnim->dCurrentTime = pPrimaryAnim->fEndTime; 
					}
				}
			}
		} 

		// prep a secondary animation if it's been activated
		if( pSecondaryAnim )
		{
			pSecondaryAnim->animTimer += (fDelta*1000.0f);

			// find out current time from the animation timer
			pSecondaryAnim->dCurrentTime = pSecondaryAnim->animTimer;//pSecondaryAnim->animTimer.GetTimeInMilliseconds();

			// should the current time be offset from the start frame (anims usually start at frame 0 though)
			float fTimeOffset = (float)((( nStartFrame )*1000) / pSecondaryAnim->fAnimationFramerate );
			pSecondaryAnim->dCurrentTime += fTimeOffset; 

			if( (pSecondaryAnim->nTotalFrames > 0) && (pSecondaryAnim->nTotalFrames < pSecondaryAnim->nTotalFrames) )
				pSecondaryAnim->fEndTime = (float)( ( (pSecondaryAnim->nTotalFrames)*1000) / pSecondaryAnim->fAnimationFramerate );
			else 
				pSecondaryAnim->fEndTime = (float)pSecondaryAnim->dTotalAnimationTime;

			// get the animation status, has it finished?
			if( pSecondaryAnim->dCurrentTime > pSecondaryAnim->fEndTime )
			{
				// animation has finished, check for a polled animation
				if( pSecondaryPolledAnim )
				{
					pSecondaryAnim = pSecondaryPolledAnim;

					if( pSecondaryAnim->bLooping )
					{
						pSecondaryAnim->nCurrentFrame = 0;
						//pSecondaryAnim->animTimer.Reset();
						pSecondaryAnim->animTimer = 0.0f;
						pSecondaryAnim->dCurrentTime = fTimeOffset;
					}
					else
					{
						pSecondaryAnim->dCurrentTime = pPrimaryAnim->fEndTime; 
					}

					// clear out the poll animation
					pSecondaryPolledAnim = 0;
				}
				else
				{
					if( !m_Paused )
					{
						if( pSecondaryAnim->bLooping )
						{
							pSecondaryAnim->nCurrentFrame = 0;
							//pSecondaryAnim->animTimer.Reset();
							pSecondaryAnim->animTimer = 0.0f;
							pSecondaryAnim->dCurrentTime = fTimeOffset;
						}
						else
						{
							// should never really be a looping animation for secondary
							pSecondaryAnim = 0;
						}
					}
				}
			} 
		}

		// go through the joints and transform/rotate
		for( i = 0; i < nNumBoneJoints; i++ )
		{
			// allow enough time to pass before updating the frame
			nFrame = pPrimaryAnim->nCurrentFrame;
			if( !m_Paused )
			{
				while( (nFrame < pPrimaryAnim->nTotalFrames) && (pPrimaryAnim->pFrames[i][nFrame].fTime < pPrimaryAnim->dCurrentTime) )
				{
					nFrame++;
				}
			}
			pPrimaryAnim->nCurrentFrame = nFrame;

			if( nFrame == 0 )
			{
				// first frame
				vPrimaryTranslate = pPrimaryAnim->pFrames[i][0].vTranslate;
				qPrimaryRotation = pPrimaryAnim->pFrames[i][0].qRotation;
			}
			else if( nFrame == pPrimaryAnim->nTotalFrames )
			{
				// last frame
				vPrimaryTranslate = pPrimaryAnim->pFrames[i][nFrame-1].vTranslate;
				qPrimaryRotation = pPrimaryAnim->pFrames[i][nFrame-1].qRotation;
			}
			else
			{
				// between first and last frame 
				curFrame = pPrimaryAnim->pFrames[i][nFrame];
				prevFrame = pPrimaryAnim->pFrames[i][nFrame-1];

				// find a point between the previous and current frame to move to
				vCurPos = pPrimaryAnim->pFrames[i][nFrame].vTranslate;
				vPrevPos = pPrimaryAnim->pFrames[i][nFrame-1].vTranslate;

				vPrimaryTranslate = Lerp( vPrevPos, vCurPos, fDelta );
				//vPrimaryTranslate = vPrevPos + (vCurPos - vPrevPos) * fDelta;

				// use quaternions
				qPrev = pPrimaryAnim->pFrames[i][nFrame-1].qRotation;
				qCur = pPrimaryAnim->pFrames[i][nFrame].qRotation;

				qPrimaryRotation = Slerp( qPrev, qCur, fDelta );
			}


			// if there's a secondary animation enabled update it
			if( pSecondaryAnim )
			{
				// allow enough time to pass before updating the frame
				nFrame = pSecondaryAnim->nCurrentFrame;
				if( !m_Paused )
				{
					while( (nFrame < pSecondaryAnim->nTotalFrames) && (pSecondaryAnim->pFrames[i][nFrame].fTime < pSecondaryAnim->dCurrentTime) )
					{
						nFrame++;
					}
				}
				pSecondaryAnim->nCurrentFrame = nFrame;

				if( nFrame == 0 )
				{
					// first frame
					vSecondaryTranslate = pSecondaryAnim->pFrames[i][0].vTranslate;
					qSecondaryRotation = pSecondaryAnim->pFrames[i][0].qRotation;
				}
				else if( nFrame == pSecondaryAnim->nTotalFrames )
				{
					// last frame
					vSecondaryTranslate = pSecondaryAnim->pFrames[i][nFrame-1].vTranslate;
					qSecondaryRotation = pSecondaryAnim->pFrames[i][nFrame-1].qRotation;
				}
				else
				{
					// between first and last frame 
					curFrame = pSecondaryAnim->pFrames[i][nFrame];
					prevFrame = pSecondaryAnim->pFrames[i][nFrame-1];

					// find a point between the previous and current frame to move to
					vCurPos = pSecondaryAnim->pFrames[i][nFrame].vTranslate;
					vPrevPos = pSecondaryAnim->pFrames[i][nFrame-1].vTranslate;
					
					vSecondaryTranslate = Lerp( vPrevPos, vCurPos, fDelta );
					//vSecondaryTranslate = vPrevPos + (vCurPos - vPrevPos) * fDelta;

					// use quaternions
					qPrev = pSecondaryAnim->pFrames[i][nFrame-1].qRotation;
					qCur = pSecondaryAnim->pFrames[i][nFrame].qRotation;

					qSecondaryRotation = Slerp( qPrev, qCur, fDelta );
				}
			}
		
			// combine the translation and rotation into a matrix
			if( pSecondaryAnim )
			{
				// anim1 * weight + (1-anim2) * weight?

				// has this animation rotated this joint?
				if( pBoneJoints[i].qRotation.X - qSecondaryRotation.X != 0.0f && 
					pBoneJoints[i].qRotation.Y - qSecondaryRotation.Y != 0.0f &&
					pBoneJoints[i].qRotation.Z - qSecondaryRotation.Z != 0.0f && 
					pBoneJoints[i].qRotation.W - qSecondaryRotation.W != 0.0f )
				{
					// it did rotate, set that movement to the bone joint
					pBoneJoints[i].relative = math::Matrix4x4( qSecondaryRotation );
				}
				else
				{
					math::Quaternion qAdd( qSecondaryRotation - pBoneJoints[i].qRotation ); 
					pBoneJoints[i].relative = math::Matrix4x4( qPrimaryRotation+qAdd );
				}

				// has this animation translated this joint?
				if( pBoneJoints[i].vTranslate.X - vSecondaryTranslate.X != 0.0f && 
					pBoneJoints[i].vTranslate.Y - vSecondaryTranslate.Y != 0.0f &&
					pBoneJoints[i].vTranslate.Z - vSecondaryTranslate.Z != 0.0f )
				{
					// it did translate, set that movement to the bone joint
					pBoneJoints[i].relative.SetTranslation( vSecondaryTranslate );
				}
				else
				{
					math::Vec3 vAdd( vSecondaryTranslate - pBoneJoints[i].vTranslate ); 
					pBoneJoints[i].relative.SetTranslation( vPrimaryTranslate+vAdd );
				}
			}

			//else
			{
				if( pPrimaryAnim && !pSecondaryAnim )
				{
					// add bone controller rotation
					if( pBoneControllers[i].nBoneIndex != -1 )
					{
						qPrimaryRotation *= pBoneControllers[i].qRotation;
						qPrimaryRotation.normalise();
					}

					pBoneJoints[i].relative = math::Matrix4x4( qPrimaryRotation );
					pBoneJoints[i].relative.SetTranslation( vPrimaryTranslate );
				}
				else if( pSecondaryAnim )
				{
					// has this animation rotated this joint?
					if( pBoneJoints[i].qRotation.X - qSecondaryRotation.X != 0.0f && 
						pBoneJoints[i].qRotation.Y - qSecondaryRotation.Y != 0.0f &&
						pBoneJoints[i].qRotation.Z - qSecondaryRotation.Z != 0.0f && 
						pBoneJoints[i].qRotation.W - qSecondaryRotation.W != 0.0f )
					{
						// add bone controller rotation
						if( pBoneControllers[i].nBoneIndex != -1 )
						{
							qSecondaryRotation *= pBoneControllers[i].qRotation;
							qSecondaryRotation.normalise();
						}

						// it did rotate, set that movement to the bone joint
						pBoneJoints[i].relative = math::Matrix4x4( qSecondaryRotation );
					}
					else
					{
						math::Quaternion qAdd( qSecondaryRotation - pBoneJoints[i].qRotation ); 

						// add bone controller rotation
						if( pBoneControllers[i].nBoneIndex != -1 )
						{
							qAdd *= pBoneControllers[i].qRotation;
							qAdd.normalise();
						}

						pBoneJoints[i].relative = math::Matrix4x4( qPrimaryRotation+qAdd );
					}

					// has this animation translated this joint?
					if( pBoneJoints[i].vTranslate.X - vSecondaryTranslate.X != 0.0f && 
						pBoneJoints[i].vTranslate.Y - vSecondaryTranslate.Y != 0.0f &&
						pBoneJoints[i].vTranslate.Z - vSecondaryTranslate.Z != 0.0f )
					{
						// it did translate, set that movement to the bone joint
						pBoneJoints[i].relative.SetTranslation( vSecondaryTranslate );
					}
					else
					{
						math::Vec3 vAdd( vSecondaryTranslate - pBoneJoints[i].vTranslate ); 
						pBoneJoints[i].relative.SetTranslation( vPrimaryTranslate+vAdd );
					}
				}
			}

			// if bone has a parent, multiply the child by the parent
			if ( pBoneJoints[i].nParentJointIndex != -1 )
			{
				pBoneJoints[i].absolute.SetMatrix( pBoneJoints[pBoneJoints[i].nParentJointIndex].absolute.m  );
				pBoneJoints[i].absolute = pBoneJoints[i].relative * pBoneJoints[i].absolute;
			}
			else
				pBoneJoints[i].absolute.SetMatrix( pBoneJoints[i].relative.m  );

			// assign results to a final matrix to update the verts
			pBoneJoints[i].final.SetMatrix( pBoneJoints[i].absolute.m );
		}
		
	}
	else
	{
		// go through the joints and just set the bind pose
		for( i = 0; i < nNumBoneJoints; i++ )
		{
			// if bone has a parent, multiply the child by the parent
			if ( pBoneJoints[i].nParentJointIndex != -1 )
			{
				pBoneJoints[i].absolute.SetMatrix( pBoneJoints[pBoneJoints[i].nParentJointIndex].absolute.m  );
				pBoneJoints[i].absolute = pBoneJoints[i].relative * pBoneJoints[i].absolute;
			}
			else
				pBoneJoints[i].absolute.SetMatrix( pBoneJoints[i].relative.m  );

			// assign results to a final matrix to update the verts
			pBoneJoints[i].final.SetMatrix( pBoneJoints[i].absolute.m );
		}
	}

	// go through each skin
	for ( i = 0; i < nNumSkins; i++ )
	{
		// update the mesh if it's enabled
		if( pSkinMeshList[i].bDraw )
		{
			for ( j = 0; j < pSkinMeshList[i].nNumPoints; j++ )
			{
				int nJointIndex0 = pSkinMeshList[i].pVertexWeights[j][0].nBoneIndex;
				float fWeight0 = pSkinMeshList[i].pVertexWeights[j][0].fWeight;

				int nJointIndex1 = pSkinMeshList[i].pVertexWeights[j][1].nBoneIndex;
				float fWeight1 = pSkinMeshList[i].pVertexWeights[j][1].fWeight;

				int nJointIndex2 = pSkinMeshList[i].pVertexWeights[j][2].nBoneIndex;
				float fWeight2 = pSkinMeshList[i].pVertexWeights[j][2].fWeight;

				int nJointIndex3 = pSkinMeshList[i].pVertexWeights[j][3].nBoneIndex;
				float fWeight3 = pSkinMeshList[i].pVertexWeights[j][3].fWeight;

				// move each vertex by the joints final matrix
				// nJointIndex0 and nWeight0 should always be valid, as there is always at least one bone affecting the vertex
				pSkinMeshList[i].pVertices[j] = math::Vec3( 0.0f, 0.0f, 0.0f );
				//pSkinMeshList[i].pPerPointNormals[j] = math::Vec3( 0.0f, 0.0f, 0.0f );

				math::Vec3 vInfluence = math::TransformByMatrix4x4( pSkinMeshList[i].pVertexWeights[j][0].vTransformedVertex, &pBoneJoints[ nJointIndex0 ].final.m[0] );
				vInfluence *= fWeight0;
				pSkinMeshList[i].pVertices[j] += vInfluence;

				math::Vec3 newRot;
				math::RotateVector( pBoneJoints[ nJointIndex0 ].final.m, pSkinMeshList[i].pVertexWeights[j][0].vTransformedNormal, newRot);
				newRot *= fWeight0;
				pSkinMeshList[i].pPerPointNormals[j] += newRot;

				// ADD POSSIBLE WEIGHT 2
				if( fWeight1 > 0.0f && nJointIndex1 != -1 )
				{
					// move vert
					vInfluence = math::TransformByMatrix4x4( pSkinMeshList[i].pVertexWeights[j][1].vTransformedVertex, &pBoneJoints[ nJointIndex1 ].final.m[0]  );
					vInfluence *= fWeight1;
					pSkinMeshList[i].pVertices[j] += vInfluence;

					math::RotateVector( pBoneJoints[ nJointIndex1 ].final.m, pSkinMeshList[i].pVertexWeights[j][1].vTransformedNormal, newRot);
					newRot *= fWeight1;
					pSkinMeshList[i].pPerPointNormals[j] += newRot;
				}
				// ADD POSSIBLE WEIGHT 3
				if( fWeight2 > 0.0f && nJointIndex2 != -1 )
				{
					// move vert
					vInfluence = math::TransformByMatrix4x4( pSkinMeshList[i].pVertexWeights[j][2].vTransformedVertex, &pBoneJoints[ nJointIndex2 ].final.m[0]  );
					vInfluence *= fWeight2;
					pSkinMeshList[i].pVertices[j] += vInfluence;

					math::RotateVector( pBoneJoints[ nJointIndex2 ].final.m, pSkinMeshList[i].pVertexWeights[j][2].vTransformedNormal, newRot);
					newRot *= fWeight2;
					pSkinMeshList[i].pPerPointNormals[j] += newRot;
				}
				// ADD POSSIBLE WEIGHT 4
				if( fWeight3 > 0.0f && nJointIndex3 != -1 )
				{
					// move vert
					vInfluence = math::TransformByMatrix4x4( pSkinMeshList[i].pVertexWeights[j][3].vTransformedVertex, &pBoneJoints[ nJointIndex3 ].final.m[0]  );
					vInfluence *= fWeight3;
					pSkinMeshList[i].pVertices[j] += vInfluence;

					math::RotateVector( pBoneJoints[ nJointIndex3 ].final.m, pSkinMeshList[i].pVertexWeights[j][3].vTransformedNormal, newRot);
					newRot *= fWeight3;
					pSkinMeshList[i].pPerPointNormals[j] += newRot;
				}

				pSkinMeshList[i].pPerPointNormals[j].normalise();
			}
		}
	}

}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void ModelHGA::Release( void )
{
	unsigned int i=0;

	if( pBoneControllers )
	{
		delete[] pBoneControllers;
		pBoneControllers = 0;
	}

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

	// Model shutdown
	Shutdown();

	// reset data
	Initialise();
}

/////////////////////////////////////////////////////
/// Method: ChangeTextureOnMaterial
/// Params: [in]meshIndex, [in]materialIndex, [in]textureIndex
///
/////////////////////////////////////////////////////
void ModelHGA::ChangeTextureOnMaterial( unsigned int meshIndex, unsigned int materialIndex, unsigned int textureIndex )
{
	DBG_ASSERT( ((meshIndex >= 0) && (meshIndex < nNumSkins)) );
	DBG_ASSERT( ((materialIndex >= 0) && (materialIndex < nNumMaterials)) );
	DBG_ASSERT( ((textureIndex >= 0) && (textureIndex < nNumTextures)) );

	pSkinMeshList[meshIndex].pMaterials[0].texUnit[0].nTextureID = m_TextureIDList[textureIndex];
}

/////////////////////////////////////////////////////
/// Method: ChangeTextureOnMaterialId
/// Params: [in]meshIndex, [in]materialIndex, [in]textureIndex
///
/////////////////////////////////////////////////////
void ModelHGA::ChangeTextureOnMaterialId( unsigned int meshIndex, unsigned int materialIndex, unsigned int textureIndex )
{
	DBG_ASSERT( ((meshIndex >= 0) && (meshIndex < nNumSkins)) );
	DBG_ASSERT( ((materialIndex >= 0) && (materialIndex < nNumMaterials)) );

	if( textureIndex != renderer::INVALID_OBJECT )
		pSkinMeshList[meshIndex].pMaterials[materialIndex].texUnit[0].bEnabled = true;
	else
		pSkinMeshList[meshIndex].pMaterials[materialIndex].texUnit[0].bEnabled = false;

	pSkinMeshList[meshIndex].pMaterials[materialIndex].texUnit[0].nTextureID = textureIndex;
}

/////////////////////////////////////////////////////
/// Method: CopyMaterials
/// Params: None
///
/////////////////////////////////////////////////////
void ModelHGA::CopyMaterials( void )
{
	unsigned int i = 0, j = 0;

	// go through each mesh
	for( i = 0; i < nNumSkins; i++ )
	{
		// for each material
		for( j = 0; j < pSkinMeshList[i].nNumMaterialTriangleLists; j++ )
		{
			// re-copy the material information
			if( pSkinMeshList[i].pMaterialIndexList[j] != -1 )
				std::memcpy( &pSkinMeshList[i].pMaterials[j], &pMaterials[ pSkinMeshList[i].pMaterialIndexList[j] ], sizeof( TMaterialEx ) );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SetupBindPose
/// Params: None
///
/////////////////////////////////////////////////////
void ModelHGA::SetupBindPose( void )
{
	unsigned int i = 0, j = 0;

	// setup each joint to the bind pose
	for ( i = 0; i < nNumBoneJoints; i++ )
	{
		int nParentIndex = pBoneJoints[i].nParentJointIndex;

		// create a relative matrix from the joints position and orientation
		pBoneJoints[i].relative = math::Matrix4x4( pBoneJoints[i].qRotation );
		pBoneJoints[i].relative.SetTranslation( pBoneJoints[i].vTranslate );

		// if this bone has a parent, multiply the relative by the parent absolute
		if ( pBoneJoints[i].nParentJointIndex != -1 )
		{
			pBoneJoints[i].absolute.SetMatrix( pBoneJoints[nParentIndex].absolute.m  );
			pBoneJoints[i].absolute = pBoneJoints[i].relative * pBoneJoints[i].absolute;
		}
		else
		{
			pBoneJoints[i].absolute.SetMatrix( pBoneJoints[i].relative.m  );
		}

		pBoneJoints[i].final.SetMatrix( pBoneJoints[i].absolute.m );
	}

		// go through each mesh and setup the vert points to be in the bind pose
	for ( i = 0; i < nNumSkins; i++ )
	{
		for ( j = 0; j < pSkinMeshList[i].nNumPoints; j++ )
		{
			int nJointIndex0 = pSkinMeshList[i].pVertexWeights[j][0].nBoneIndex;
			int nJointIndex1 = pSkinMeshList[i].pVertexWeights[j][1].nBoneIndex;
			int nJointIndex2 = pSkinMeshList[i].pVertexWeights[j][2].nBoneIndex;
			int nJointIndex3 = pSkinMeshList[i].pVertexWeights[j][3].nBoneIndex;


			// WEIGHT VERTEX 1
			if( nJointIndex0 != -1 )
			{
				math::Matrix4x4 matrix = pBoneJoints[ nJointIndex0 ].final;

				// create the vertex position
				pSkinMeshList[i].pVertexWeights[j][0].vTransformedVertex = pSkinMeshList[i].pPoints[j];
				pSkinMeshList[i].pVertexWeights[j][0].vTransformedNormal = pSkinMeshList[i].pPerPointNormals[j];

				// move only the POINTS for the bind pose
				matrix.InverseTranslation( pSkinMeshList[i].pVertexWeights[j][0].vTransformedVertex );
				matrix.InverseRotate( pSkinMeshList[i].pVertexWeights[j][0].vTransformedVertex );

				matrix.InverseRotate( pSkinMeshList[i].pVertexWeights[j][0].vTransformedNormal );
			}

			// WEIGHT VERTEX 2
			if( nJointIndex1 != -1 )
			{
				math::Matrix4x4 matrix = pBoneJoints[ nJointIndex1 ].final;

				// create the vertex position
				pSkinMeshList[i].pVertexWeights[j][1].vTransformedVertex = pSkinMeshList[i].pPoints[j];
				pSkinMeshList[i].pVertexWeights[j][1].vTransformedNormal = pSkinMeshList[i].pPerPointNormals[j];

				// move only the POINTS for the bind pose
				matrix.InverseTranslation( pSkinMeshList[i].pVertexWeights[j][1].vTransformedVertex );
				matrix.InverseRotate( pSkinMeshList[i].pVertexWeights[j][1].vTransformedVertex );

				matrix.InverseRotate( pSkinMeshList[i].pVertexWeights[j][1].vTransformedNormal );
			}

			// WEIGHT VERTEX 3
			if( nJointIndex2 != -1 )
			{
				math::Matrix4x4 matrix = pBoneJoints[ nJointIndex2 ].final;

				// create the vertex position
				pSkinMeshList[i].pVertexWeights[j][2].vTransformedVertex = pSkinMeshList[i].pPoints[j];
				pSkinMeshList[i].pVertexWeights[j][2].vTransformedNormal = pSkinMeshList[i].pPerPointNormals[j];

				// move only the POINTS for the bind pose
				matrix.InverseTranslation( pSkinMeshList[i].pVertexWeights[j][2].vTransformedVertex );
				matrix.InverseRotate( pSkinMeshList[i].pVertexWeights[j][2].vTransformedVertex );

				matrix.InverseRotate( pSkinMeshList[i].pVertexWeights[j][2].vTransformedNormal );
			}

			// WEIGHT VERTEX 4
			if( nJointIndex3 != -1 )
			{
				math::Matrix4x4 matrix = pBoneJoints[ nJointIndex3 ].final;

				// create the vertex position
				pSkinMeshList[i].pVertexWeights[j][3].vTransformedVertex = pSkinMeshList[i].pPoints[j];
				pSkinMeshList[i].pVertexWeights[j][3].vTransformedNormal = pSkinMeshList[i].pPerPointNormals[j];

				// move only the POINTS for the bind pose
				matrix.InverseTranslation( pSkinMeshList[i].pVertexWeights[j][3].vTransformedVertex );
				matrix.InverseRotate( pSkinMeshList[i].pVertexWeights[j][3].vTransformedVertex );

				matrix.InverseRotate( pSkinMeshList[i].pVertexWeights[j][3].vTransformedNormal );
			}
		}
	}

	pPrimaryAnim = 0;
	pPolledAnim = 0;
	pSecondaryAnim = 0;
}

/////////////////////////////////////////////////////
/// Method: DrawBones
/// Params: None
///
/////////////////////////////////////////////////////
void ModelHGA::DrawBones( void )
{
	unsigned int i;

	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();
	bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();

	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	if( lightState )
		renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	
	for( i = 0; i < nNumBoneJoints; i++ )
	{
		if( pBoneJoints[i].nParentJointIndex != -1 )
		{
			renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 0, 255 );

			math::Vec3 vaPoints[2];	

			vaPoints[0] = math::Vec3( pBoneJoints[pBoneJoints[i].nParentJointIndex].final.m41, pBoneJoints[pBoneJoints[i].nParentJointIndex].final.m42, pBoneJoints[pBoneJoints[i].nParentJointIndex].final.m43 );
			vaPoints[1] = math::Vec3( pBoneJoints[i].final.m41, pBoneJoints[i].final.m42, pBoneJoints[i].final.m43 );

			//glVertexPointer(3, GL_FLOAT, sizeof(math::Vec3), vaPoints);
			glDrawArrays( GL_LINES, 0, 2 );

			renderer::OpenGL::GetInstance()->SetColour4ub( 255, 0, 0, 255 );
			vaPoints[0] = math::Vec3(pBoneJoints[i].final.m41, pBoneJoints[i].final.m42, pBoneJoints[i].final.m43);
			//glVertexPointer(3, GL_FLOAT, sizeof(math::Vec3), vaPoints);
			glDrawArrays( GL_POINTS, 0, 1 );
		}
	}

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );

	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();

	if( lightState )
		renderer::OpenGL::GetInstance()->EnableLighting();
}

/////////////////////////////////////////////////////
/// Method: SetSkinDrawState
/// Params: [in]szSkinName
///
/////////////////////////////////////////////////////
void ModelHGA::SetSkinDrawState( int skinId, bool state )
{
	unsigned int i=0;

	if( skinId != -1 )
	{
		DBG_ASSERT( skinId < static_cast<int>(nNumSkins) );

		pSkinMeshList[skinId].bDraw = state;
	}
	else
	{
		for( i=0; i < nNumSkins; ++i )
			pSkinMeshList[i].bDraw = state;
	}
}

/////////////////////////////////////////////////////
/// Method: SetAnimation
/// Params: [in]nAnimId, [in]nFrameOffset, [in]bForceChange
///
/////////////////////////////////////////////////////
void ModelHGA::SetAnimation( unsigned int nAnimId, int nFrameOffset, bool bForceChange )
{
	unsigned int i=0;
	// requesting the same animation for primary and secondary ?
	if( pSecondaryAnim != 0 )
		if( nAnimId == pSecondaryAnim->nAnimId )
			return;

	if( pPrimaryAnim )
	{
		// if it asks for the same animation and the animation is looping then there's no need to change
		if( ( pPrimaryAnim->nAnimId == nAnimId ) && (pPrimaryAnim->bLooping || bForceChange==false) )
		{
			return;
		}
	}

	// forcing a change, update the primary animation pointer
	if( bForceChange )
	{
		for( i=0; i < BoneAnimationList.size(); ++i )
		{
			if( BoneAnimationList[i]->nAnimId == nAnimId  )
			{
				pPrimaryAnim = BoneAnimationList[i];
				if( nFrameOffset != -1 && nFrameOffset < pPrimaryAnim->nTotalFrames )
					pPrimaryAnim->nCurrentFrame = nFrameOffset;
				else
					pPrimaryAnim->nCurrentFrame = 0;
				//pPrimaryAnim->animTimer.Reset();
				//pPrimaryAnim->animTimer.Start();
				pPrimaryAnim->animTimer = 0.0f;
				return;
			}
		}
	}
	else
	{
		// not forcing a change so enable the poll animation
		for( i=0; i < BoneAnimationList.size(); ++i )
		{
			if( BoneAnimationList[i]->nAnimId == nAnimId )
			{
				pPolledAnim = BoneAnimationList[i];
				if( nFrameOffset != -1 && nFrameOffset < pPolledAnim->nTotalFrames )
					pPolledAnim->nCurrentFrame = nFrameOffset;
				else
					pPolledAnim->nCurrentFrame = 0;
				//pPolledAnim->animTimer.Reset();
				//pPolledAnim->animTimer.Start();
				return;
			}
		}
	}

	// got this far, the animation doesn't exist so just set the bind pose
	pPrimaryAnim = 0;
}

/////////////////////////////////////////////////////
/// Method: SetFrame
/// Params: [in]nAnimId, [in]nFrame
///
/////////////////////////////////////////////////////
void ModelHGA::SetFrame( unsigned int nAnimId, int nFrame )
{
	unsigned int i=0;

	// forcing a change, update the primary animation pointer
	for( i=0; i < BoneAnimationList.size(); ++i )
	{
		if( BoneAnimationList[i]->nAnimId == nAnimId )
		{
			pPrimaryAnim = BoneAnimationList[i];
			if( nFrame != -1 && nFrame < pPrimaryAnim->nTotalFrames )
				pPrimaryAnim->nCurrentFrame = nFrame;
			else
				pPrimaryAnim->nCurrentFrame = 0;
			//pPrimaryAnim->animTimer.Reset();
			//pPrimaryAnim->animTimer.Start();
			pPrimaryAnim->animTimer = 0.0f;
			return;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: BlendAnimation
/// Params: [in]nAnimId, [in]fWeight, [in]bForceChange
///
/////////////////////////////////////////////////////
void ModelHGA::BlendAnimation( unsigned int nAnimId, float fWeight, bool bForceChange )
{
	unsigned int i=0;

	// a secondary anim is already playing and don't want to force a change ?
	//if( pSecondaryAnim != 0 && bForceChange == false )
	//	return;

	// already playing the anim ?
	if( pSecondaryAnim != 0 && !bForceChange )
		if( pSecondaryAnim->nAnimId == nAnimId ) 
			return;

	// requesting the same animation for primary and secondary ?
	if( pPrimaryAnim != 0 )
		if( nAnimId == pPrimaryAnim->nAnimId )
			return;

	if( bForceChange )
	{
		for( i=0; i < BoneAnimationList.size(); ++i )
		{
			if( BoneAnimationList[i]->nAnimId == nAnimId)
			{
				pSecondaryAnim = BoneAnimationList[i];

				pSecondaryAnim->nCurrentFrame = 0;
				//pSecondaryAnim->animTimer.Reset();
				//pSecondaryAnim->animTimer.Start();
				pSecondaryAnim->animTimer = 0.0f;
				pSecondaryAnim->fBlendWeight = fWeight;
				return;
			}
		}
	}
	else
	{
		for( i=0; i < BoneAnimationList.size(); ++i )
		{
			if( BoneAnimationList[i]->nAnimId == nAnimId)
			{
				pSecondaryPolledAnim = BoneAnimationList[i];

				pSecondaryPolledAnim->nCurrentFrame = 0;
				//pSecondaryPolledAnim->animTimer.Reset();
				//pSecondaryPolledAnim->animTimer.Start();
				pSecondaryPolledAnim->fBlendWeight = fWeight;
				return;
			}
		}
	}

	// got this far, the animation doesn't exist so just set the bind pose
	pSecondaryAnim = 0;
}

/////////////////////////////////////////////////////
/// Method: SetBoneControllerRotation
/// Params: [in]szBoneName, [in]q
///
/////////////////////////////////////////////////////
void ModelHGA::SetBoneControllerRotation( int index, math::Quaternion &q )
{
	unsigned int i = 0;

	for( i = 0; i < nNumBoneJoints; i++ )
	{
		if( pBoneJoints[i].nJointIndex == index )
		{
			pBoneControllers[i].nBoneIndex = i;
			pBoneControllers[i].qRotation = q;
			return;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ClearBlend
/// Params: None
///
/////////////////////////////////////////////////////
void ModelHGA::ClearBlend()
{
	pSecondaryAnim = 0;
	pSecondaryPolledAnim = 0;
}

#endif // BASE_SUPPORT_OPENGL_GLSL
