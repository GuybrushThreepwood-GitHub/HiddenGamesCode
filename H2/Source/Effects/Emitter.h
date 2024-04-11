
/*===================================================================
	File: Emitter.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __EMITTER_H__
#define __EMITTER_H__

#include "CollisionBase.h"

namespace efx
{
	class Emitter
	{
		public:
			struct EmitterBlock
			{
				int spriteId;

				bool cameraAlign;
				bool yRotationOnly;

				bool usePoints;
				bool smoothPoints;
				bool useHWPointSprites;
	
				/// max number of spawnable particles
				int maxParticles;

				// depth
				bool enableDepth;
				GLenum depthFunc;

				/// blend 
				bool enableBlending;
				GLenum blendSrc;
				GLenum blendDest;

				/// alpha
				bool enableAlpha;
				GLenum alphaCompare;
				float alphaClamp;

				// particle system attributes
				/// gravity value
				math::Vec3 gravity;

				// these vars determine starting values for each particle
				math::Vec2 emitRate; // in particles/sec
				math::Vec2 lifetime; // in sec
				
				bool useScaling;
				math::Vec2 size;

				bool useRandomColour;

				bool useWidthHeightDepth;
				math::Vec3 dims;

				math::Vec3 spawnDir1;
				math::Vec3 spawnDir2;

				math::Vec3 emitRadius;

				math::Vec4 startColour1;
				math::Vec4 startColour2;

				math::Vec4 endColour1;
				math::Vec4 endColour2;
			};
			
			enum
			{
				EMITTERSTATE_STARTON=0,
				EMITTERSTATE_STARTOFF
			};

		public:
			/// default constructor
			Emitter();
			/// default destructor
			~Emitter();

			/// Create - Creates an emitter system
			/// \param particleCount - max number of particles in the system
			/// \param pointSize - particle size
			/// \param texID - particle GL texture ID
			/// \return integer ( SUCCESS: OK or FAIL: FAIL )
			int Create( unsigned int particleCount, float pointSize=1.0f, GLuint texID=renderer::INVALID_OBJECT );
			/// Create - Creates an emitter system
			/// \param emitterBlock - block containing creation data
			int Create( EmitterBlock& emitterBlock );

			/// Draw - Draws all the particles in the system
			void Draw( void );
			void DrawDebug( void );

			/// Update - Updates all the particles in the system
			/// \param fTime - time since last frame update
			void Update( float deltaTime );

			/// Stop - removes all particles in the system
			void Stop( void );
			/// Reset - resets the system to initial values
			/// \return integer ( SUCCESS: OK or FAIL: FAIL )
			int Reset( void );

			/// DestroySystem - destroys and frees this systems data
			/// \return integer ( SUCCESS: OK or FAIL: FAIL )
			int Destroy( void );

			/// SetTextureUVOffset - Creates uv offsets from start and end pixel points
			/// \param nX - bottom left x pixel point
			/// \param nY - bottom left y pixel point
			/// \param nWidth - width of the image
			/// \param nHeight - height of the image
			/// \param nTextureWidth - width of the texture
			/// \param nTextureHeight - height of the texture
			void SetTextureUVOffset( int nX, int nY, int nWidth, int nHeight, int nTextureWidth, int nTextureHeight );
			/// SetTextureUVOffset - Creates uv offsets from start and end pixel points
			/// \param uOffset - bottom left x pixel point
			/// \param vOffset - bottom left y pixel point
			/// \param w - width of the image
			/// \param h - height of the image
			void SetTextureUVOffset( float uOffset, float vOffset, float w, float h );

			// attributes
			void Enable()	
			{ 
				if( !m_EmitEnabled )
				{
					m_EmitEnabled = true; 
					Reset(); 

					if( m_TimeOn != -1.0f )
					{
						m_CurrentTimer = 0.0f;
						m_TimeToWait = m_TimeOn;
					}
				}
			}
			void Disable()	
			{
				if( m_EmitEnabled )
				{
					m_EmitEnabled = false; 

					if( m_TimeOff != -1.0f )
					{
						m_CurrentTimer = 0.0f;
						m_TimeToWait = m_TimeOff;
					}
				}
			}
	
			bool IsEnabled()							{ return m_EmitEnabled; } 

			/// GetGravity - returns the current gravity vector
			/// \return Vec3 ( SUCCESS: gravity vector)
			math::Vec3 GetGravity(void) const { return(m_Gravity); }
			/// SetGravity - Sets the current gravity vector
			/// \param data - new gravity vector
			void SetGravity(const math::Vec3 &data) { m_Gravity = data; }

			/// GetPos - returns the current position vector
			/// \return Vec3 ( SUCCESS: position vector)
			const math::Vec3& GetPos(void) const { return(m_Pos); }
			/// SetPos - Sets the current position vector
			/// \param data - new position vector
			void SetPos(const math::Vec3 &data) { m_Pos = data; }

			/// GetRot - returns the current rotation vector
			/// \return Vec3 ( SUCCESS: position vector)
			const math::Vec3& GetRot(void) const { return(m_Rot); }
			/// SetRot - Sets the current rotation vector
			/// \param data - new position vector
			void SetRot(const math::Vec3 &data) { m_Rot = data; }

			/// GetPos - returns the current max/min emit rate
			/// \return CMinMax<float> ( SUCCESS: max/min emit rate )
			math::CMinMax<float> GetEmitRate(void) const { return(m_EmitRate); }
			/// SetPos - Sets the max/min emit rate
			/// \param data - new max/min emit rate
			void SetEmitRate(const math::CMinMax<float> data) { m_EmitRate = data; }

			/// GetLifetime - returns the current max/min lifetime of a particle
			/// \return CMinMax<float> ( SUCCESS: max/min lifetime of a particle )
			math::CMinMax<float> GetLifetime(void) const { return(m_Lifetime); }
			/// SetLifetime - Sets the current max/min lifetime of a particle
			/// \param data - new max/min lifetime of a particle
			void SetLifetime(const math::CMinMax<float> data) { m_Lifetime = data; }

			/// GetSize - returns the current max/min size of a particle
			/// \return CMinMax<float> ( SUCCESS: max/min size of a particle )
			math::CMinMax<float> GetSize(void) const { return(m_Size); }
			/// SetSize - Sets the current max/min size of a particle
			/// \param data - new max/min size of a particle
			void SetSize(const math::CMinMax<float> data) { m_Size = data; }

			/// GetUseWidthHeightDepth - returns if the width/height/depth params are in use
			/// \return bool ( SUCCESS: flag state )
			bool GetUseWidthHeightDepth(void) const { return(m_UseWidthHeightDepth); }
			/// SetUseWidthHeightDepth - Sets if the width/height/depth params are used
			/// \param data - state of the flag
			void SetUseWidthHeightDepth(bool data) { m_UseWidthHeightDepth= data; }

			/// GetDimensions - returns the width/height params
			/// \return bool ( SUCCESS: dimensions )
			const math::Vec3& GetDimensions(void) const { return(m_Dims); }
			/// SetDimensions - Sets the width/height params
			/// \param data - dimensions to set
			void SetDimensions( const math::Vec3& data) { m_Dims = data; }

			/// SetUseScaling - Sets if the scaling param is used
			/// \param data - state of the flag
			void SetUseScaling(bool data) { m_UseScaling = data; }
			/// SetUseRandomColour - Sets if the colours are random
			/// \param data - state of the flag
			void SetUseRandomColour(bool data) { m_UseRandomColour = data; }

			/// GetStartColour1 - returns the current start colour 1 of a particle
			/// \return Vec4 ( SUCCESS: start colour 1 of a particle )
			math::Vec4 GetStartColour1(void) const { return(m_StartColour1); }
			/// SetStartColour1 - Sets the current start colour 1 of a particle
			/// \param data - new start colour 1 of a particle 
			void SetStartColour1(const math::Vec4 &data) { m_StartColour1 = data; }

			/// GetStartColour2 - returns the current start colour 2 of a particle
			/// \return Vec4 ( SUCCESS: start colour 2 of a particle )
			math::Vec4 GetStartColour2(void) const { return(m_StartColour2); }
			/// SetStartColour2 - Sets the current start colour 2 of a particle
			/// \param data - new start colour 2 of a particle 
			void SetStartColour2(const math::Vec4 &data) { m_StartColour2 = data; }

			/// GetEndColour1 - returns the current end colour 1 of a particle
			/// \return Vec4 ( SUCCESS: end colour 1 of a particle )
			math::Vec4 GetEndColour1(void) const { return(m_EndColour1); }
			/// SetEndColour1 - Sets the current end colour 1 of a particle
			/// \param data - new end colour 1 of a particle 
			void SetEndColour1(const math::Vec4 &data) { m_EndColour1 = data; }

			/// GetEndColour2 - returns the current end colour 2 of a particle
			/// \return Vec4 ( SUCCESS: end colour 2 of a particle )
			math::Vec4 GetEndColour2(void) const { return(m_EndColour2); }
			/// SetEndColour2 - Sets the current end colour 2 of a particle
			/// \param data - new end colour 2 of a particle 
			void SetEndColour2(const math::Vec4 &data) { m_EndColour2 = data; }

			/// GetSpawnDir1 - returns the current spawn direction 1 of a particle
			/// \return Vec3 ( SUCCESS: spawn direction 1 of a particle )
			math::Vec3 GetSpawnDir1(void) const { return(m_SpawnDir1); }
			/// SetSpawnDir1 - Sets the current spawn direction 1
			/// \param data - new spawn direction 1
			void SetSpawnDir1(const math::Vec3 &data) { m_SpawnDir1 = data; }

			/// GetSpawnDir2 - returns the current spawn direction 2 of a particle
			/// \return Vec3 ( SUCCESS: spawn direction 2 of a particle )
			math::Vec3 GetSpawnDir2(void) const { return(m_SpawnDir2); }
			/// SetSpawnDir2 - Sets the current spawn direction 2
			/// \param data - new spawn direction 2
			void SetSpawnDir2(const math::Vec3 &data) { m_SpawnDir2 = data; }

			/// GetEmitRadius - returns the current emit radius of a particle
			/// \return Vec3 ( SUCCESS: emit radius of a particle )
			math::Vec3 GetEmitRadius(void) const { return(m_EmitRadius); }
			/// SetEmitRadius - Sets the current emit radius
			/// \param data - new emit radius
			void SetEmitRadius(const math::Vec3 &data) { m_EmitRadius = data; }

			/// GetTimeOn - returns the current emitter time on
			/// \return float ( SUCCESS: emitter time on )
			float GetTimeOn(void) const { return(m_TimeOn); }
			/// SetTimeOn - Sets the current emitter time on
			/// \param data - new emit radius
			void SetTimeOn( float time ) { m_TimeOn = time; }

			/// GetTimeOff - returns the current emitter time on
			/// \return float ( SUCCESS: emitter time on )
			float GetTimeOff(void) const { return(m_TimeOff); }
			/// SetTimeOn - Sets the current emitter time on
			/// \param time - new emit radius
			void SetTimeOff( float time ) { m_TimeOff = time; }
			/// SetTimeToWait - Sets the current wait timer
			/// \param time - new time
			void SetTimeToWait( float time ) { m_TimeToWait = time; }

			/// SetTexture - Sets the particles to a GL texture ID
			/// \param nTexID - GL texture ID
			void SetTexture( GLuint nTexID) { m_TextureID = nTexID; }
			/// GetTexture - Gets the particles to a GL texture ID
			/// \return GLuint - GL texture ID
			GLuint GetTexture(void) const { return(m_TextureID); }

			/// GetMaxParticles - returns the maximum number of particles in this system
			/// \return integer ( SUCCESS: max number of particles )
			int GetMaxParticles(void) const { return(m_MaxParticles); }

			/// GetSpriteId - returns the sprite id
			/// \return integer ( SUCCESS: sprite id )
			int GetSpriteId(void) const { return(m_SpriteId); }

		private:
			void MoveParticle( int i, int triSpriteOffset, float deltaTime );
			void UpdateAndClampColour( int vertIndex, int particleIndex, float deltaTime );
			void SetupVertexBufferObject();
			void DrawVertexBufferObject();

			void SetupVA();
			void DrawVA();

		private:
			struct Particle
			{
				Particle()
				{
					size = 1.0f;
					dims = math::Vec2( 1.0f, 1.0f );
					lifetime = 1.0f;
					age = 0.0f;
				  
					center		= math::Vec3( 0.0f, 0.0f, 0.0f );
					colour		= math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
					colourStep	= math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
					sizeStep	= 1.0f;
					dir			= math::Vec3( 0.0f, 1.0f, 0.0f );
				}
				/// particle size
				float size;
				/// particle dimensions when not square
				math::Vec3 dims;
				/// particle life
				float lifetime;
				/// particle age
				float age;

				/// particle center
				math::Vec3 center;
				/// particle colour
				math::Vec4 colour;
				/// particle colour step
				math::Vec4 colourStep;
				/// current direction of particle
				math::Vec3 dir;
				// size step
				float sizeStep;
			};

			struct ParticleVert
			{
				math::Vec3 v;
				math::Vec2 uv;
				math::Vec4Lite col;
			};

			int m_SpriteId;

			bool m_AllParticlesDisabled;
			int m_ParticleDisableCount;

			bool m_CameraAlign;
			bool m_UsePoints;
			bool m_SmoothPoints;
			bool m_UseHWPointSprites;
			bool m_YRotationOnly;

			/// main particle list pointer
			Particle*  m_pParticles;

			/// particles position
			//math::Vec3* m_pParticlePositions;
			/// particles texture coords
			//math::Vec2* m_pParticleTextureCoords;
			/// particles colour buffer
			//math::Vec4* m_pParticleColours;

			ParticleVert* m_ParticleVertList;

			/// point sprite size
			float m_PointSize;

			math::Vec3 m_Right;
			math::Vec3 m_Up;

			bool m_EmitEnabled;

			/// view align flag
			bool m_ViewAlign;
			/// depth flag
			bool m_EnableDepth;
			/// depth function
			GLenum m_DepthFunc;
			/// blend flag
			bool m_EnableBlending;
			/// blend source
			GLenum m_BlendSrc;
			/// blend destination
			GLenum m_BlendDest;
			/// alpha flag
			bool m_EnableAlpha;
			/// alpha compare mode
			GLenum m_AlphaCompare;
			/// alpha clamp value
			float m_AlphaClamp;

			// particle system attributes
			/// system gravity value
			math::Vec3 m_Gravity;
			/// system position
			math::Vec3 m_Pos;
			/// system rotation
			math::Vec3 m_Rot;

			// these vars determine starting values for each particle
			math::CMinMax<float> m_EmitRate; // in particles/sec
			math::CMinMax<float> m_Lifetime; // in particles/sec
			math::CMinMax<float> m_Size; // in particles/sec

			bool m_UseWidthHeightDepth;
			math::Vec3 m_Dims;

			bool m_UseScaling;

			bool m_UseRandomColour;
			bool m_RandomColourSwap;

			/// possible spawn colour 1
			math::Vec4 m_StartColour1;
			/// possible spawn colour 2
			math::Vec4 m_StartColour2;
			/// possible spawn end colour 1
			math::Vec4 m_EndColour1;
			/// possible spawn end colour 2
			math::Vec4 m_EndColour2;
			/// possible spawn direction 1
			math::Vec3 m_SpawnDir1;
			/// possible spawn direction 2
			math::Vec3 m_SpawnDir2;
			/// emit radius of system
			math::Vec3 m_EmitRadius;
			/// max number of spawnable particles
			int m_MaxParticles;
			/// GL texture id for system
			GLuint m_TextureID;

			/// system bounding box values
			collision::AABB m_SystemBoundingBox;
			/// system bounding sphere values
			collision::Sphere m_SystemBoundingSphere;

			/// GL VBO vertex object id for this system
			GLuint m_PointBufferObject;
			/// GL VBO 2D texture object id for this system
			GLuint m_TextureBufferObject2D;
			/// GL VBO vertex colour object id for this system
			GLuint m_ColourBufferObject;

			/// VBO initialised flag
			bool m_VBOInitialised;	

			float m_TimeOn;
			float m_TimeOff;
			float m_CurrentTimer;
			float m_TimeToWait;

			math::Vec2 m_UVCoords[4];

			math::Vec4 m_GeneratedStartColour;
			math::Vec4 m_GeneratedEndColour;

			math::Vec3 m_BoxMinSize;
			math::Vec3 m_BoxMaxSize;
	};

} // namespace efx

#endif // __EMITTER_H__

