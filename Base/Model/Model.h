
#ifndef __MODEL_H__
#define __MODEL_H__

#ifdef BASE_SUPPORT_OPENGL_GLSL
	#define MODEL_MAX_USEABLE_TEXUNITS	4	
#else
	#define MODEL_MAX_USEABLE_TEXUNITS	3	
#endif //

#include <vector>

// forward declare
namespace collision { class Sphere; class AABB; }
namespace mdl { 
					struct TextureHeader;
					struct MaterialHeader; 
					struct TextureFormat;
					struct MaterialFormat;
					
					namespace transform
					{
						struct TransformBlock;
					}
				}
namespace renderer { class Texture; }

	struct TTextureUnit
	{
		/// unit enable flag
		int bEnabled;	
		/// texture lookup index
		int		nTexLookup;
		/// GL texture object id
		unsigned int nTextureID;
		/// texture has alpha flag
		int		bTexHasAlpha;
		/// texture matrix scale X
		float fTextureScaleX;
		/// texture matrix scale Z
		float fTextureScaleZ;
	};

	struct TMaterialEx
	{
		/// material index
		int		nIndex;
		/// texture offset tracker
		int		nTextureOffset;
		/// material diffuse colour
		math::Vec4	Diffuse;	
		/// material ambient colour
		math::Vec4	Ambient;	
		/// material specular colour
		math::Vec4	Specular;	
		/// material emissive colour
		math::Vec4	Emissive;	
		/// material shininess value
		float	Shininess;

		// texture units for this material
		TTextureUnit texUnit[MODEL_MAX_USEABLE_TEXUNITS];
	};

namespace mdl
{
	// forward declare
	class SkinMesh;
	class Mesh;

	/// general flags
	const unsigned long MODELFLAG_GENERAL_DRAWMODEL				= 0x000000001;
	const unsigned long MODELFLAG_GENERAL_AUTOCULLING			= 0x000000002;

	const unsigned long MODELFLAG_GENERAL_AABBCULL				= 0x000000010;
	const unsigned long MODELFLAG_GENERAL_SPHERECULL			= 0x000000020;
	const unsigned long MODELFLAG_GENERAL_SUBMESHCULL			= 0x000000040;
	const unsigned long MODELFLAG_GENERAL_MODELCULL				= 0x000000080;

	const int BONEJOINTNAME_SIZE		= 32;
	const int ANIMATIONNAME_SIZE		= 32;
	const int MAX_WEIGHTS_PER_VERTEX	= 4;

	/// structure containing valid/supported draw modes
	enum EModelDrawModes
	{
		MODELDRAWMODE_VBO=0,
		MODELDRAWMODE_VA,
	};

	//////////////////////////////////////////////////////

	/// structure containing vertex weight
	struct TVertexWeights
	{
		/// bone influence index
		int		nBoneIndex;
		/// weight value
		float	fWeight;
		/// transformed vertex
		math::Vec3 vTransformedVertex;
		/// transformed normal
		math::Vec3 vTransformedNormal;
	};

	//////////////////////////////////////////////////////

	/// structure containing bone joint based storage values
	struct TBoneJoint
	{
		/// position of the bone joint
		math::Vec3		vTranslate;
		/// rotation of the bone joint
		math::Quaternion		qRotation;
		/// scale of the bone
		math::Vec3		vScale;

		/// index lookup for parent
		int				nParentJointIndex;
		/// index lookup for this joint
		int				nJointIndex;

		/// absolute matrix of joint
		math::Matrix4x4		absolute; 
		/// relative position of joint to parent
		math::Matrix4x4		relative;
		/// final matrix of joint
		math::Matrix4x4		final;

		TBoneJoint &operator = (const TBoneJoint &b)
		{
			vTranslate			= b.vTranslate;
			qRotation			= b.qRotation;
			vScale				= b.vScale;
			nParentJointIndex	= b.nParentJointIndex;
			nJointIndex			= b.nJointIndex;

			absolute			= b.absolute; 
			relative			= b.relative;
			final				= b.final;

			return (*this);
		}

	};

	//////////////////////////////////////////////////////

	/// structure containing bone joint keyframe storage values
	struct TBoneJointKeyframe
	{
		/// time in seconds
		float fTime;		
		/// position of the bone joint
		math::Vec3		vTranslate;
		/// rotation of the bone joint
		math::Quaternion		qRotation;
		/// scale of the bone
		math::Vec3		vScale;

		/// quaternion rotation of the bone joint
		math::Quaternion		qRotationOrientation;
		/// orientation of the bone joint
		math::Quaternion		qJointOrientation;

	};

	//////////////////////////////////////////////////////

	/// structure containing bone joint controller details
	struct TBoneJointController
	{
		/// bone index
		int nBoneIndex;		

		/// rotation of the bone joint
		math::Quaternion		qRotation;

	};

	//////////////////////////////////////////////////////

	class BoneAnimation
	{
		public:
			/// default constructor
			BoneAnimation();
			/// default destructor
			~BoneAnimation();

			/// bone animation file path
			file::TFile AnimationFile;
			/// animation timer
			float animTimer;
			// animation id
			unsigned int nAnimId;
			/// framerate of the animation
			float fAnimationFramerate;
			/// total amount of time this animation plays for
			double dTotalAnimationTime;
			/// total number of joints exported into this file
			unsigned int nTotalJoints;
			/// total number of frames in this animation
			int nTotalFrames;
			/// timer for tracking this animation
			//core::app::CTimer animTimer;
			/// looping flag
			bool bLooping;
			/// current active frame
			int nCurrentFrame;
			/// current time
			double dCurrentTime;
			/// end of the animation time
			float fEndTime;
			/// current blend weight
			float fBlendWeight;
			/// pointer to a pointer of all the keyframes for each bone joint
			TBoneJointKeyframe **pFrames;
	};

	//////////////////////////////////////////////////////

	/// SetMaterialData - Sets the current parameters for a materials texture
	/// \param nTexUnit - unit to set the parameters on
	/// \param pMaterial - pointer to material
	void SetMaterialData( int nTexUnit, TMaterialEx *pMaterial );

	/// ResetMaterialData - Resets the current parameters for a materials texture
	/// \param nTexUnit - unit to set the parameters on
	/// \param pMaterial - pointer to material
	void ResetMaterialData( int nTexUnit, TMaterialEx *pMaterial );

	//////////////////////////////////////////////////////

	class Model
	{
		public:
			/// default constructor
			Model();
			/// default destructor
			virtual ~Model();

			/// Init - Should setup any of the models member variables
			void Init( void );
			/// Initialise - Should setup any of the models member variables
			virtual void Initialise( void );
			/// Load - Should Load the models data from a file
			/// \param szFilename - name of file to load
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			virtual int Load( const char *szFilename )	{ return(1); }
			/// Draw - Should draw the model
			virtual void Draw( void )					{}
			/// Update - Should update the model every frame
			/// \param fDelta - time since last frame
			virtual void Update( float fDelta )			{}
			/// FlushData - Flushes any allocated values
			void FlushData( void );
			/// Release - Should remove all memory from the model
			virtual void Release( void )				{}
			/// Shutdown - This function should be called in the virutal release function to auto free the data members that are allocated
			int Shutdown( void );

			/// DrawBoundingBox - draws the models bounding box calculated from its loaded vertices
			void DrawBoundingBox( void );
			/// DrawBoundingSphere - draws the models bounding sphere calculated from its loaded vertices
			void DrawBoundingSphere( void );
			/// SetModelFlags - Changes the models general flags to allow custom drawing of certain aspects
			/// \param newFlags - the flag to turn on/off
			void SetModelGeneralFlags( unsigned int newFlags  );
			/// SetModelDrawFlags - Changes the models draw flags to allow custom drawing of certain aspects
			/// \param newFlags - the flag to turn on/off
			/// \param bToggle - toggle option to switch the flag parameter on or off
			void SetModelDrawFlags( unsigned int newFlags, bool bToggle  );
			/// SetActiveProgram - Sets the active program for the entire model
			/// \param programId - the shader program id
			void SetActiveProgram( GLuint programId );
			/// GetModelPath - Gets the model path
			/// \return integer - model path
			const char *GetModelPath( void ) const	{ return ModelFile.szPath; }
			/// GetNumVertices - Gets the number of vertices in the model
			/// \return integer - number of vertices
			unsigned int GetNumVertices( void ) const	{ return nNumVertices; }
			/// GetNumNormals - Gets the number of normals in the model
			/// \return integer - number of normals
			unsigned int GetNumNormals( void ) const		{ return nNumNormals; }
			/// GetNumMaterials - Gets the number of materials in the model
			/// \return integer - number of materials
			unsigned int GetNumMaterials( void ) const	{ return nNumMaterials; }
			/// GetNumTextures - Gets the number of textures in the model
			/// \return integer - number of textures
			unsigned int GetNumTextures( void ) const	{ return nNumTextures; }
			/// GetNumTexCoords - Gets the number of texture coordinates in the model
			/// \return integer - number of texture coordinates
			unsigned int GetNumTexCoords( void ) const	{ return nNumTextureCoords; }
			/// GetNumBoneJoints - Gets the number of bone joints in the model
			/// \return integer - number of bone joints
			unsigned int GetNumBoneJoints( void ) const	{ return nNumBoneJoints; }

			/// GetNumSkinMeshes - Gets the number of skinned meshes in the model
			/// \return integer - number of skinned meshes
			unsigned int GetNumSkinMeshes( void ) const	{ return nNumSkins; }
			/// GetNumMeshes - Gets the number of meshes in the model
			/// \return integer - number of meshes
			unsigned int GetNumOpaqueMeshes( void ) const	{ return nNumOpaqueMeshes; }
			/// GetNumMeshes - Gets the number of meshes in the model
			/// \return integer - number of meshes
			unsigned int GetNumSortedMeshes( void ) const	{ return nNumSortedMeshes; }

			/// GetVerticesPtr - Gets a pointer to the start of the vertex data
			/// \return Vec3 pointer - vertex data start
			math::Vec3 *GetVerticesPtr( void ) const				{ return pVertices; }
			/// GetNormalsPtr - Gets a pointer to the start of the normals
			/// \return Vec3 pointer - normal data start
			math::Vec3 *GetNormalsPtr( void ) const					{ return pNormals; }
			/// GetTexCoordsPtr - Gets a pointer to the start of the texture coordinates
			/// \return Vec2 pointer - texture coordinate start
			math::Vec2 *GetTexCoordsPtr( void ) const				{ return pTexCoords; }

			/// GetVertexColourPtr - Gets a pointer to the start of the materials
			/// \return TMaterialEx pointer - material data start
			TMaterialEx *GetMaterialsPtr( void ) const		{ return pMaterials; }
			/// GetBoneJointsPtr - Gets a pointer to the start of the bone joints
			/// \return TBoneJoint pointer - bone joint data start
			TBoneJoint *GetBoneJointsPtr( void ) const		{ return pBoneJoints; }
			/// GetSkinMeshListPtr - Gets a pointer to the start of the skin mesh list
			/// \return SkinMesh * - skin mesh list data start
			SkinMesh *GetSkinMeshListPtr( void ) const		{ return pSkinMeshList; }
			/// GetOpaqueMeshListPtr - Gets a pointer to the start of the mesh list
			/// \return Mesh * - mesh list data start
			Mesh *GetOpaqueMeshListPtr( void ) const		{ return pOpaqueMeshList; }
			/// GetSortedMeshListPtr - Gets a pointer to the start of the mesh list
			/// \return Mesh * - mesh list data start
			Mesh *GetSortedMeshListPtr( void ) const		{ return pSortedMeshList; }

			/// GetVertexBufferObject - Get the models GL VBO vertex id
			/// \return unsigned int ( SUCCESS: GL vbo id or FAIL: INVALID_OBJECT )
			GLuint GetVertexBufferObject( void ) const		{ return nVertexBufferObject; }
			/// GetTextureBufferObject - Get the models GL VBO texture id
			/// \return unsigned int ( SUCCESS: GL vbo id or FAIL: INVALID_OBJECT )
			GLuint GetTextureBufferObject2D( void ) const		{ return nTextureBufferObject2D; }
			/// GetNormalBufferObject - Get the models GL VBO normal id
			/// \return unsigned int ( SUCCESS: GL vbo id or FAIL: INVALID_OBJECT )
			GLuint GetNormalBufferObject( void ) const		{ return nNormalBufferObject; }

			/// GetBoneAnimationList - Get the bone animation list for the skeleton
			/// \return  <BoneAnimation* > ( SUCCESS: animation list or FAIL: 0 )
			std::vector <BoneAnimation* > GetBoneAnimationList( void ) const	{ return (BoneAnimationList); }

		protected:
			mdl::TextureHeader m_TextureHeader;
			mdl::MaterialHeader m_MaterialHeader;
		
			std::vector<mdl::TextureFormat> m_TextureFormatList;
			std::vector<mdl::MaterialFormat> m_MaterialFormatList;

			std::vector<GLuint> m_TextureIDList;
			std::vector<mdl::Material> m_MaterialBlockList;

			renderer::Texture m_TextureLoader;

			/// GL VBO vertex object id for this model
			GLuint nVertexBufferObject;
			/// GL VBO 2D texture object id for this model
			GLuint nTextureBufferObject2D;
			/// GL VBO normal object id for this model
			GLuint nNormalBufferObject;

			/// current shader program
			GLuint nActiveProgram;

			/// VBO initialised flag
			bool bVBOInitialised;	

			/// has vertices stored flag
			bool bHasVertices;
			/// has texture coords stored flag
			bool bHasTextureCoords;
			/// has normals stored flag
			bool bHasNormals;
			/// sort mesh by id
			bool bSortMeshes;

			/// number of vertex colours stored
			unsigned int nNumVertexColours;
			/// number of vertices stored
			unsigned int nNumVertices;
			/// number of points stored
			unsigned int nNumPoints;
			/// number of normals stored
			unsigned int nNumNormals;
			/// number of textures stored
			unsigned int nNumTextures;
			/// number of texture coords stored
			unsigned int nNumTextureCoords;

			/// number of bone joints stored
			unsigned int nNumBoneJoints;
			/// number of materials stored
			unsigned int nNumMaterials;
			/// number of bone animations stored
			unsigned int nNumBoneAnimations;
			/// number of skinned meshes stored
			unsigned int	nNumSkins;
			/// number of meshes stored
			unsigned int	nNumOpaqueMeshes;
			/// number of meshes stored
			unsigned int	nNumSortedMeshes;

			/// main point list
			math::Vec3		*pPoints;
			/// main vertices list
			math::Vec3		*pVertices;
			/// main normal list
			math::Vec3		*pNormals;
			/// main texture coord list
			math::Vec2			*pTexCoords;

			/// main material list
			TMaterialEx		*pMaterials;
			/// animation flag
			int				bHasAnimation;
			/// main bone joint list
			TBoneJoint		*pBoneJoints;

			/// list of bone animations
			std::vector<BoneAnimation *> BoneAnimationList;
			/// main skinned mesh list
			SkinMesh		*pSkinMeshList;
			/// main static mesh list
			Mesh			*pOpaqueMeshList;
			/// main static mesh list
			Mesh			*pSortedMeshList;

		public:
			/// model file size
			size_t			nFileSize;
			/// model general flags
			unsigned int	nGeneralFlags;
			/// model draw flags
			unsigned int	nDrawFlags;
			/// model data flush flags
			unsigned int	nDataFlushFlags;
			/// model position
			math::Vec3		vPos;
			/// model rotation
			math::Vec3		vRot;
			/// model orientation
			math::Quaternion		qOri;
			/// model file holder
			file::TFile			ModelFile;
			
			/// flag to use sub mesh culling states
			int				bUseSubMeshFog;

			/// model bounding AABB
			collision::AABB			modelAABB;
			/// model bounding sphere
			collision::Sphere			modelSphere;

			/// model draw flag
			int				bDraw;
			/// model valid load flag
			int				bValidLoad;

		private:
	};
} // namespace mdl

#endif // __MODEL_H__


