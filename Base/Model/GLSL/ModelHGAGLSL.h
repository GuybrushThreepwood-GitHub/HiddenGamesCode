
#ifndef __MODELHGAGLSL_H__
#define __MODELHGAGLSL_H__

namespace mdl
{
	class ModelHGA : public Model
	{
		public:
			/// default constructor
			ModelHGA();
			/// default destructor
			virtual ~ModelHGA();

			/// Initialise - Should initialise the models data
			virtual void Initialise( void );
			/// Load - Loads a HGA file
			/// \param szFilename - name of file to load
			/// \param magFilter - magnification on textures
			/// \param minFilter - minification on textures
			/// \return integer - (SUCCESS:OK or FAIL:FAIL)
			virtual int Load( const char *szFilename, GLenum magFilter=GL_LINEAR, GLenum minFilter=GL_LINEAR, bool lowPower=false );
			/// LoadBindPose - Loads the bind pose data for the model
			/// \param szFilename - name of file to load
			/// \return integer - (SUCCESS:OK or FAIL:FAIL)
			int LoadBindPose( const char *szFilename, GLenum magFilter=GL_LINEAR, GLenum minFilter=GL_LINEAR, bool lowPower=false );
			/// SetupAndFree - Preps the VBO data
			void SetupAndFree();
			/// LoadAnimation - Loads an animation for the model
			/// \param szFilename - name of file to load
			/// \param nAnimId - animation id
			/// \param bLoopFlag - can this animation loop
			/// \return integer - (SUCCESS:OK or FAIL:FAIL)
			int LoadAnimation( const char *szFilename, unsigned int nAnimId, bool bLoopFlag );
			/// Draw - Draws all the models data
			virtual void Draw( void );
			/// DrawShadowMap - Draws a model as a shadow map
			void DrawShadowMap( void );
			/// Update - Updates all the models data
			/// \param fDelta - time since last frame update
			virtual void Update( float fDelta );
			/// Release - Clears any memory used by the model
			virtual void Release( void );
	//		int Shutdown( void );

			/// ChangeTextureOnMaterial - changes the texture on a material for a particular skin
			/// \param meshIndex - which mesh id
			/// \param materialIndex - which material
			/// \param textureIndex - which texture index
			void ChangeTextureOnMaterial( unsigned int meshIndex, unsigned int materialIndex, unsigned int textureIndex );

			/// ChangeTextureOnMaterialId - changes the texture on a material for a particular skin but using a passed texture id
			/// \param meshIndex - which mesh id
			/// \param materialIndex - which material
			/// \param textureIndex - passed texture index
			void ChangeTextureOnMaterialId( unsigned int meshIndex, unsigned int materialIndex, unsigned int textureIndex );

			/// CopyMaterials - Copies the main material list over to each material used by the sub mesh data
			void CopyMaterials( void );
			/// SetupBindPose - Initialises all the joints, matrix and vertex data
			void SetupBindPose( void );
			/// DrawBones - Draws the current bone setup as points and lines
			void DrawBones( void );
			/// DisableSkinDraw - Allows you to disables any meshes defined in a bind pose so they aren't updated or drawn
			/// \param skinId - which skin to set the state on
			/// \param state - the state to set
			void SetSkinDrawState( int skinId, bool state );
			/// SetAnimation - Sets the current animation for the bone data
			/// \param nAnimId - animation id
			/// \param nFrameOffset - defines a frame offset from the usual start of this animation
			/// \param bForceChange - Allows the current requested animation to be changed instantly if true
			void SetAnimation( unsigned int nAnimId, int nFrameOffset = -1, bool bForceChange = true );
			/// SetFrame - Sets the current animation frame
			/// \param nAnimId - animation id
			/// \param nFrame - frame to set
			void SetFrame( unsigned int nAnimId, int nFrame );
			/// BlendAnimation - Blends another animation over the current one playing
			/// \param nAnimId - animation id
			/// \param fWeight - defines how much influence the animation should have over the current one
			/// \param bForceChange - Allows the current requested animation to be changed instantly if true
			void BlendAnimation( unsigned int nAnimId, float fWeight = 1.0f, bool bForceChange = false );
			/// SetBoneController - Sets the value of a bone controller
			/// \param index - which bone
			/// \param q - quaternion rotation
			void SetBoneControllerRotation( int index, math::Quaternion &q );
			/// ClearBlend - Clears any blend animation data
			void ClearBlend();

			/// GetPrimaryAnim - gets the primary animation pointer
			/// \return BoneAnimation * - pointer to the animation
			BoneAnimation *GetPrimaryAnim()	{ return pPrimaryAnim; }
			/// GetPolledAnim - gets the polled animation pointer
			/// \return BoneAnimation * - pointer to the animation
			BoneAnimation *GetPolledAnim()	{ return pPolledAnim; }
			/// GetSecondaryAnim - gets the secondary animation pointer
			/// \return BoneAnimation * - pointer to the animation
			BoneAnimation *GetSecondaryAnim()	{ return pSecondaryAnim; }
			/// GetPrimaryAnimId - Get the primary animation
			/// \return unsigned int - animation id
			unsigned int GetPrimaryAnimId()		{ return pPrimaryAnim->nAnimId; }

			/// SetPauseFlag - Pauses the models animation
			/// \param bPause - pause flag
			void SetPauseFlag( bool bPause )					{ m_Paused =  bPause; }

		public:
			/// character file details
			file::TFile CharacterFile;
			/// config file details
			file::TFile ConfigFile;
			/// bind pose file details
			file::TFile BindPoseFile;

		private:
			/// total poly count of this HGM
			unsigned int m_TotalPolyCount;
			/// total triangle count of this HGM
			unsigned int m_TotalTriCount;

			/// bind pose file loaded flag
			bool m_BindPoseFileLoaded;
			/// offset into reference file of bone data
			long nFileBoneOffset;
			/// pause flag
			bool m_Paused;

			/// current primary animation
			BoneAnimation *pPrimaryAnim;
			/// current polled animation
			BoneAnimation *pPolledAnim;
			/// current secondary (blend) animation
			BoneAnimation *pSecondaryAnim;
			/// current polled animation
			BoneAnimation *pSecondaryPolledAnim;
			/// bone controller list
			TBoneJointController *pBoneControllers;
	};

} // namespace mdl

#endif // __MODELHGAGLSL_H__


