
#ifndef __SPRITE_H__
#define __SPRITE_H__

#ifdef BASE_SUPPORT_OPENGL

namespace renderer
{
	class Sprite
	{
		public:

			/// default constructor
			Sprite();
			/// constructor
			/// \param nSizeX - width of the sprite
			/// \param nSizeY - height of the sprite
			/// \param nTexID - GL texture id
			/// \param vPosition - center point of the sprite
			/// \param bAlign - should the sprite be aligned to the camera
			/// \param bYRotOnly - should the sprite only align to the camera on the Y axis
			/// \param bUsePointSprites - should it be forced to draw using point sprites
			Sprite( int nSizeX, int nSizeY, GLuint nTexID, const math::Vec3 &vPosition, bool bAlign, bool bYRotOnly, bool bUsePointSprites );
			/// default destructor
			virtual ~Sprite();

			/// OPERATOR 'ASSIGN' - Assigned the data of parameters sprite to 'this' sprite
			/// \param s - sprite to assign
			/// \return Sprite ( SUCCESS: 'this' sprite )
			Sprite			&operator =  (const Sprite &s);

			/// Draw - Draws the sprite
			virtual void Draw( void );
			/// SetPosition - sets the center point of the sprite
			/// \param vNewPos - new position of sprite
			void SetPosition( const math::Vec3 &vNewPos );
			/// SetPosition - sets the center point of the sprite
			/// \param X - new X position of the sprite
			/// \param Y - new Y position of the sprite
			/// \param Z - new Z position of the sprite
			void SetPosition( float X, float Y, float Z );
			/// SetRotation - sets the angular rotation of the sprite
			/// \param vNewRot - new rotation of sprite
			void SetRotation( const math::Vec3 &vNewRot );
			/// SetRotation - sets the angular rotation of the sprite
			/// \param X - new X rotation of the sprite
			/// \param Y - new Y rotation of the sprite
			/// \param Z - new Z rotation of the sprite
			void SetRotation( float X, float Y, float Z );
			/// SetWidthHeight - Sets the width and height of the sprite
			/// \param nNewWidth - new sprite width
			/// \param nNewHeight - new sprite height
			void SetWidthHeight( int nNewWidth, int nNewHeight );
			/// SetTexture - Sets the GL texture ID of the sprite image
			/// \param nTexID - GL texture ID
			void SetTexture( GLuint nTexID );
			/// SetTextureUVOffset - Creates uv offsets from start and end pixel points
			/// \param nX - bottom left x pixel point
			/// \param nY - bottom left y pixel point
			/// \param nWidth - width of the image
			/// \param nHeight - height of the image
			/// \param nTextureWidth - width of the texture
			/// \param nTextureHeight - height of the texture
			void SetTextureUVOffset( int nX, int nY, int nWidth, int nHeight, int nTextureWidth, int nTextureHeight );
			/// ViewAlign - Sets the camera alignment flag of the sprite
			/// \param bAlign - alignment flag
			void ViewAlign( bool bAlign );
			/// SetColour - Sets the colour of the sprite
			/// \param vNewColour - colour data 
			void SetColour( math::Vec4Lite vNewColour )				{ vColour = vNewColour; }
			/// GetColour - Gets the colour of the sprite
			/// \return Vec4 - colour data 
			math::Vec4Lite GetColour( void )					{ return(vColour); }

			/// sprite position
			math::Vec3 vPos;
			/// sprite rotation
			math::Vec3 vRot;
			/// sprite depth value
			float fDepth;
			/// sprite width
			int nWidth;
			/// sprite height
			int nHeight;
			/// sprite GL texture object id
			GLuint nTexID;
			/// use point sprite flag
			bool bPointSprite;
			/// view align flag
			bool bViewAlign;
			/// Y rotation only flag
			bool bYRotationOnly;
			/// double sided flag
			bool bDoubleSided;
			/// use colour flag
			bool bColour;
			/// colour vector
			math::Vec4Lite vColour;
			/// UV coords
			math::Vec2 vUVCoords[4];

			/// texture wrap mode
			float fSWrapMode;
			/// texture wrap mode
			float fTWrapMode;

			/// texture scale factor along S
			float fScaleS;
			/// texture scale factor along T
			float fScaleT;

			/// texture minification filter
			float fMinFilter;
			/// texture magnification filter
			float fMagFilter;

			/// enable mipmap flag
			bool bUseDefaultMipmapping;
			/// mipmap base level
			int nMipmapBaseLevel;
			/// mipmap max level
			int nMipmapMaxLevel;
			/// mipmap min lod
			float fMipmapMinLod;
			/// mipmap max lod
			float fMipmapMaxLod;
			/// lod bias value
			float fMipmapLodBias;
			/// max lod bias
			float fMipmapLodBiasMax;

			/// depth flag
			bool bEnableDepth;
			/// depth function
			GLenum eDepthFunc;
			/// blend flag
			bool bEnableBlending;
			/// blend source function
			GLenum eBlendSrc;
			/// blend destination function
			GLenum eBlendDest;
			/// alpha flag
			bool bEnableAlpha;
			/// alpha compare function
			GLenum eAlphaCompare;
			/// alpha clamp value
			float fAlphaClamp;

		private:

	};

	/// SetTextureParameters - Sets the current parameters for a sprites texture
	/// \param pSprite - pointer to sprite
	/// \param nResetData - reset flag
	void SetTextureParameters( Sprite *pSprite, int nResetData );

} // namespace renderer

#endif // BASE_SUPPORT_OPENGL

#endif // __SPRITE_H__



