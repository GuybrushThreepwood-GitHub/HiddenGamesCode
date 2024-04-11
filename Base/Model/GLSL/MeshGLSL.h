
#ifndef __MESHGLSL_H__
#define __MESHGLSL_H__

namespace mdl { namespace transform { struct TransformBlock; } }

namespace mdl
{
	class Mesh;

	/// MeshSort - compares the mesh index for sorting
	/// \param pMesh1 - pointer to first mesh
	/// \param pMesh2 - pointer to second mesh
	/// \return integer - 1 to swap, -1 not to swap
	int MeshSort( const void *pMesh1, const void *pMesh2 );
	/// MeshSortByDepth - compares the mesh index for sorting
	/// \param pMesh1 - pointer to first mesh
	/// \param pMesh2 - pointer to second mesh
	/// \return integer - 1 to swap, -1 not to swap
	int MeshSortByDepth( const void *pMesh1, const void *pMesh2 );

	class Mesh
	{
		public:
			/// default constructor
			Mesh();
			/// default destructor
			virtual ~Mesh();

			/// Initialise - Should setup any of the mesh member variables
			virtual void Initialise( void );
			/// Draw - Draws the mesh
			virtual void Draw( void );

			/// SetActiveProgram - Sets the active program for the entire model
			/// \param programId - the shader program id
			void SetActiveProgram( GLuint programId );

			/// SetupVertexBufferObject - Generates the vertex buffer objects GL ID's
			void SetupVertexBufferObject( void );
			/// DrawVertexBufferObject - Draws the mesh using vertex buffer objects
			void DrawVertexBufferObject( void );
			/// DrawVA - Draws the mesh using vertex arrays
			void DrawVA( void );
			/// SetTextureAnimation - Enable a texture animation
			void SetTextureAnimation( int numFrames, mdl::TextureAnimationBlock::AnimationData* anim );
			/// Update - update the mesh for this frame
			/// \param deltaTime - time since last frame
			void Update( float deltaTime );

			/// CalculateBounds - Calculates a bounding box/sphere for the mesh
			void CalculateBounds( void );
			/// DrawBoundingBox - Draws the meshes bounding box
			void DrawBoundingBox( void );
			/// DrawBoundingSphere - Draws the meshes bounding sphere
			void DrawBoundingSphere( void );
			/// FlushData - Flushes any allocated values
			void FlushData( void );
			/// Release - Should remove all memory from the skinned mesh
			virtual void Release( void );
			/// Shutdown - This function should be called in the virutal release function to auto free the data members that are allocated
			int Shutdown( void );

			/// mesh general flags
			unsigned int nGeneralFlags;
			/// mesh draw flags
			unsigned int nDrawFlags;
			/// mesh data flush flags
			unsigned int nDataFlushFlags;

			/// GL VBO interleaved object id for this model
			GLuint nInterleavedBufferObject;
			/// GL VBO vertex object id for this model
			GLuint nVertexBufferObject;
			/// GL VBO 2D texture object id for this model
			GLuint nTextureBufferObject2D;
			/// GL VBO normal object id for this model
			GLuint nNormalBufferObject;
			/// GL VBO element object id for this model
			GLuint nElementBufferObject;

			/// current shader program
			GLuint nActiveProgram;
			/// vertex attrib location
			GLint nVertexAttribLocation;
			/// texture coords attrib location
			GLint nTexCoordsAttribLocation;
			/// normal attrib location
			GLint nNormalAttribLocation;
			/// tangent attrib location
			GLint nTangentAttribLocation;
			/// sampler index
			GLint nTexSamplerUniform[MODEL_MAX_USEABLE_TEXUNITS];

			/// VBO initialised flag
			bool bVBOInitialised;		

			/// has vertices stored flag
			bool bHasVertices;
			/// has texture coords stored flag
			bool bHasTextureCoords;
			/// has normals stored flag
			bool bHasNormals;
			// has generated tangents
			bool bHasTangents;

			/// mesh sort flag
			bool bSortMeshes;

			/// number of vertex colour definitions
			unsigned int nNumVertexColours;
			/// number of vertex definitions
			unsigned int nNumVertices;
			/// number of normal definitions
			unsigned int nNumNormals;
			/// number of texture coord definitions
			unsigned int nNumTextureCoords;
			/// number of material definitions
			unsigned int nNumMaterials;

			/// main pointer to materials
			TMaterialEx		*pMaterials;

			/// vertex, uv, normal data holder
			InterleavedData*	pInterleavedData;

			/// number of texture animations
			int nNumTexAnimationFrames;
			/// uv animation data
			GLuint* nTexAnimInterleavedBufferObject;
			/// InterleavedData for texture animations (for each frame)
			InterleavedData**	pTexAnimInterleavedData;
			/// source frame
			GLuint nOriginalInterleavedBufferObject;

			/// number of triangle lists in this mesh
			unsigned int	nNumMaterialTriangleLists;
			/// material index list pointer
			int	*pMaterialIndexList;
			/// triangle per material number list
			unsigned int	*pTriPerMaterialList;

			/// total poly count of this mesh
			unsigned int	nTotalPolyCount;
			/// total vertex count of this mesh
			unsigned int	nTotalVertexCount;
			/// total triangle count of this mesh
			unsigned int	nTotalTriangleCount;
			/// id of the mesh
			unsigned int	nMeshId;
			/// index of the mesh (used for draw ordering)
			int	nMeshIndex;

			/// store previous draw depth/blend/alpha state
			renderer::TGLDepthBlendAlphaState	savedDepthBlendAlphaState;
			/// store previous draw cull state
			renderer::TGLCullFaceState savedCullFaceState;
			/// stored previous face winding
			GLenum savedFrontFaceWinding;

			/// store draw depth/blend/alpha state
			renderer::TGLDepthBlendAlphaState	meshDepthBlendAlphaState;
			/// store draw cull state
			renderer::TGLCullFaceState meshCullFaceState;
			/// stored face winding
			GLenum meshFrontFaceWinding;
			/// store fog settings
			renderer::TGLFogState meshFogState;

			/// sort flag
			int meshSort;

			int meshSimpleColourFlag;
			math::Vec4Lite meshSimpleColour;

			bool meshTranslate;
			math::Vec3 meshTranslation;

			bool meshRotate;
			math::Vec3 meshRotation;

			bool meshScale;
			math::Vec3 meshScaleFactors;

			// transform block
			mdl::transform::TransformBlock meshTransformData;

			// num frame
			int numTextureAnimationFrames;
			// current frame
			int currentTextureAnimationFrame;
			// texture animation timer
			float currentTextureAnimationTimer;
			// texture animation
			mdl::TextureAnimationBlock::AnimationData* pTextureAnimationData;
			// original texture
			GLuint originalTextureId;

			/// mesh AABB
			collision::AABB			meshAABB;
			/// mesh bounding sphere
			collision::Sphere			meshSphere;
			/// selection flag used in editing
			int				bIsSelected;

			/// mesh draw flag
			int				bDraw;
			/// mesh valid load flag
			int				bValidLoad;
			// mesh depth for sorting
			float			sceneDepth;

			/// shared anims only update once
			bool textureAnimUpdated;
	};

} // namespace mdl

#endif // __MESHGLSL_H__


