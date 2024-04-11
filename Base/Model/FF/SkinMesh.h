
#ifndef __SKINMESH_H__
#define __SKINMESH_H__

namespace mdl
{
	class SkinMesh
	{
		public:
			/// default constructor
			SkinMesh();
			/// default destructor
			virtual ~SkinMesh();

			/// Initialise - Should setup any of the skin mesh member variables
			virtual void Initialise( void );
			/// Draw - Draws the skinned mesh
			virtual void Draw( void );

			/// SetupVertexBufferObject - Generates the vertex buffer objects GL ID's
			void SetupVertexBufferObject( void );
			/// DrawVertexBufferObject - Draws the mesh using vertex buffer objects
			void DrawVertexBufferObject( void );
			/// DrawVA - Draws the mesh using vertex arrays
			void DrawVA( void );

			/// Release - Should remove all memory from the skinned mesh
			virtual void Release( void );
			/// Shutdown - This function should be called in the virutal release function to auto free the data members that are allocated
			int Shutdown( void );

			/// skin mesh general flags
			unsigned int nGeneralFlags;
			/// skin mesh draw flags
			unsigned int nDrawFlags;
			/// skin model data flush flags
			unsigned int nDataFlushFlags;

			/// GL VBO vertex object id for this model
			GLuint nVertexBufferObject;
			/// GL VBO 2D texture object id for this model
			GLuint nTextureBufferObject2D;
			/// GL VBO normal object id for this model
			GLuint nNormalBufferObject;
			/// GL VBO element object id for this model
			GLuint nElementBufferObject;

			/// VBO initialised flag
			bool bVBOInitialised;
			
			/// has vertices stored flag
			bool bHasVertices;
			/// has texture coords stored flag
			bool bHasTextureCoords;
			/// has normals stored flag
			bool bHasNormals;

			/// number of vertex colour definitions
			unsigned int nNumVertexColours;
			/// number of vertex definitions
			unsigned int nNumVertices;
			/// number of point definitions
			unsigned int nNumPoints;
			/// number of normal definitions
			unsigned int nNumNormals;
			/// number of texture coord definitions
			unsigned int nNumTextureCoords;
			/// number of material definitions
			unsigned int nNumMaterials;

			/// main pointer to points
			math::Vec3		*pPoints;
			/// main pointer to vertex
			math::Vec3		*pVertices;
			/// main pointer to full vertex list
			math::Vec3		*pFullVertexList;
			/// main pointer to pointer of the vertices
			math::Vec3		**ppVertexPositionPointer;
			/// main pointer to the joint index list
			int				*pJointIndexList;
			/// main pointer to weights
			float			*pWeightList;
			/// main pointer to vertex weights
			TVertexWeights	**pVertexWeights;
			/// main pointer to normals
			math::Vec3		*pPerPointNormals;
			/// main pointer to normals
			math::Vec3		*pNormals;
			/// main pointer to full normal list
			math::Vec3		*pFullNormalList;
			/// main pointer to pointer of the normals
			math::Vec3		**ppNormalPositionPointer;
			/// main pointer to texture coords
			math::Vec2		*pTexCoords;
			/// main pointer to materials
			TMaterialEx		*pMaterials;

			/// number of triangles in this skin
			unsigned int	nNumMaterialTriangleLists;
			/// material index looup list
			int	*pMaterialIndexList;
			/// number of triangles in each material list
			unsigned int	*pTriPerMaterialList;

			/// total poly count in this skin
			unsigned int	nTotalPolyCount;
			/// total vertex count in this skin
			unsigned int	nTotalVertexCount;
			/// total triangle count in this skin
			unsigned int	nTotalTriangleCount;

			/// id of the mesh
			unsigned int	nMeshId;
			/// index of the mesh (used for draw ordering)
			unsigned int	nMeshIndex;

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

			/// AABB of this skin
			collision::AABB			meshAABB;
			/// sphere around this skin
			collision::Sphere			meshSphere;
			/// selection flag used in editing
			int				bIsSelected;

			/// draw flag
			int				bDraw;
			/// valid load flag
			int				bValidLoad;
	};
} // namespace mdl
#endif // __SKINMESH_H__


