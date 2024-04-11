
#ifndef __MODELHGMGLSL_H__
#define __MODELHGMGLSL_H__

namespace mdl
{
	class ModelHGM : public Model
	{
		public:
			/// default constructor
			ModelHGM();
			/// default destructor
			virtual ~ModelHGM();
			
			/// Initialise - Initialise the models data
			virtual void Initialise( void );
			/// Load - Load a model
			/// \param szFilename - file to load
			/// \param magFilter - magnification on textures
			/// \param minFilter - minification on textures
			/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
			virtual int Load( const char *szFilename, GLenum magFilter=GL_LINEAR, GLenum minFilter=GL_LINEAR, bool lowPower=false );
			/// Draw - Draws a model
			virtual void Draw( void );

			/// Update - Updates a model
			/// \param fDelta - time since last update
			virtual void Update( float fDelta, bool meshSort=false );
			/// Release - Frees all the models data
			virtual void Release( void );

			/// LoadTextureAnimations - load texture animation data 
			/// \param blockCount - number of data blocks
			/// \param blocks - block data structures
			void LoadTextureAnimations( unsigned int blockCount, TextureAnimationBlock* blocks );
			/// SetTextureAnimation - Enables a texture animation
			/// \param animationIndex - animation index
			void SetTextureAnimation( unsigned int animationIndex );

			/// EnableDefaultMeshColour - If a model uses simple mesh colour enable this clears other meshes to white
			void EnableDefaultMeshColour()				{m_UseDefaultColour=true;}
			/// DisableDefaultMeshColour - Disables the default colour
			void DisableDefaultMeshColour()				{m_UseDefaultColour=false;}

			/// EnableMeshDefaultColour - If a model uses simple mesh colour enable this clears other meshes to white
			void EnableMeshDefaultColour( unsigned int meshId );
			/// DisableMeshDefaultColour - Disables the default colour
			void DisableMeshDefaultColour( unsigned int meshId );

			/// SetDefaultMeshColour - Sets the default colour of the entire model
			/// \param colour - colour of the mesh
			void SetDefaultMeshColour( const math::Vec4Lite& colour );
			/// SetMeshDefaultColour - Sets the default colour of the entire model
			/// \param meshId - the id of the mesh
			/// \param colour - colour of the mesh
			void SetMeshDefaultColour( unsigned int meshId, const math::Vec4Lite& colour );
			/// GetMeshDefaultColour - Sets the default colour of the entire model
			/// \param meshId - the id of the mesh
			/// \return math::Vec4Lite - colour of the mesh
			math::Vec4Lite&  GetMeshDefaultColour( unsigned int meshId );

			/// SetMeshDrawState - Sets the drawable state of a mesh
			/// \param meshId - the mesh id to set state on
			/// \param state - the state to set (TRUE-draw or FALSE-do not draw)
			void SetMeshDrawState( int meshId, bool state );

			/// SetMeshTranslation - Sets up a mesh translation
			/// \param meshId - the mesh id to set state on
			/// \param trans - translation values
			void SetMeshTranslation( int meshId, const math::Vec3& trans );
			/// SetMeshRotation - Sets up a mesh rotation
			/// \param meshId - the mesh id to set state on
			/// \param rot - rotation values
			void SetMeshRotation( int meshId, const math::Vec3& rot );
			/// SetMeshScale - Sets up a mesh scale
			/// \param meshId - the mesh id to set state on
			/// \param scale - scale values
			void SetMeshScale( int meshId, const math::Vec3& scale );

			/// SetupAndFree - Uploads the model data to GL then frees the local memory
			void SetupAndFree();

			/// CopyMaterials - copy over the global materials to the sub material list for each triangle list
			void CopyMaterials( int matIndex );

			/// GetTotalPolyCount - Gets the total poly count of the model
			/// \return integer - (SUCCESS: total polycount)
			int GetTotalPolyCount( void ) const	{ return m_TotalPolyCount; }
			/// GetTotalTriCount - Gets the total triangle count of the model
			/// \return integer - (SUCCESS: total triangle count)
			int GetTotalTriCount( void ) const	{ return m_TotalTriCount; }
		
			/// SetNumTextures - Sets the new number of textures
			void SetNumTextures( int nNumOfTextures )	{ nNumTextures = nNumOfTextures; }
			/// SetTextureToMaterial - Sets the texture id to a material
			/// \param texID - texture object id
			/// \param matIndex - material index
			/// \param texUnit - texture unit
			void SetTextureToMaterial( GLuint texID, int matIndex, int texUnit=0 );
			/// GetTextureOnMaterial - Gets the texture id on a material
			/// \param matIndex - material index
			/// \param texUnit - texture unit
			/// \return GLuint - texture object id
			GLuint GetTextureOnMaterial( int matIndex, unsigned int texUnit=0 );
			/// SetMaterialColour - Changes a material colour
			/// \param matIndex - material index
			/// \param materialParam - material parameter
			/// \param colour - colour
			void SetMaterialColour( int matIndex, GLenum materialParam, const math::Vec4& colour );
			/// SetMaterialShininess - Changes a material shininess
			/// \param matIndex - material index
			/// \param shininess - shininess value
			void SetMaterialShininess( int matIndex, float shininess );
			/// SetMaterialScale - Changes a material texture scale
			/// \param matIndex - material index
			/// \param xOffset - x offset
			/// \param zOffset - z offset
			void SetMaterialScale( int meshIndex, int matIndex, unsigned int texUnit, float xOffset, float zOffset );

			/// GetMeshFromIndex - Tries to find the mesh with the parameter index
			/// \param meshIndex - index to search for
			/// \return mdl::Mesh* - mesh pointer
			mdl::Mesh* GetMeshFromIndex( unsigned int meshIndex );

			/// GetNumAnimationBlocks - Gets the current number of texture animations
			/// \returns unsigned int - number of texture animations
			unsigned int GetNumAnimationBlocks()	{ return m_NumAnimationBlocks; }

		private:
			/// total poly count of this HGM
			unsigned int m_TotalPolyCount;
			/// total triangle count of this HGM
			unsigned int m_TotalTriCount;
			/// use mesh default colour flag
			bool m_UseDefaultColour;
			/// default mesh colour
			math::Vec4Lite m_DefaultMeshColour;

			/// animation blocks
			unsigned int m_NumAnimationBlocks;
			TextureAnimationBlock* m_TexAnimationBlocks;
	};

} // namespace mdl

#endif // __MODELHGMGLSL_H__


