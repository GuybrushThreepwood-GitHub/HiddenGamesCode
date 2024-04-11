
#ifndef __BATCHSPRITERENDER_H__
#define __BATCHSPRITERENDER_H__

// forward declare
class Player;

#if defined(BASE_PLATFORM_tvOS) || defined(BASE_PLATFORM_WINDOWS)
	const int MAX_BATCHES = 400;
	const int MAX_SPRITES_PER_BATCH = 700;
#else
	const int MAX_BATCHES = 300;
	const int MAX_SPRITES_PER_BATCH = 700;
#endif

class BatchSpriteRender
{
	public:
		/// default constructor
		BatchSpriteRender();
		/// default destructor
		~BatchSpriteRender();

		void AddSprite( int batchId, const collision::AABB& aabb, const math::Vec3& pos, const math::Vec3& dims, int spriteResId );

		/// Draw - Draws the active wave grid
		void Draw( void );
		/// Update - Updates the wave grid motion
		/// \param deltaTime - time delta from previous frame
		void Update( float deltaTime );

		void SetPlayer( Player* pPlayer )			{ m_pPlayer = pPlayer; }
		void SetAlphaBlend( bool state, float value )
		{
			m_AlphaBlend = state;
			m_AlphaBlendValue = value;
		}
		void SetMaxDrawDistance( float distance )	{ m_MaxDistance = distance; }
		void SetDrawStyle( int style )				{ m_DrawStyle = style; }
		void Clear();

	private:
		struct SpriteVert
		{
			math::Vec3 v;
			math::Vec2 uv;
			math::Vec4Lite col;
		};

		struct SpriteData
		{
			math::Vec3 pos;
			math::Vec3 dims;
			float distance;
			float radius;
		};

		struct SpriteBatch
		{
			int batchId;
			int spriteCount;
			GLuint textureId;

			collision::AABB	batchAABB;

			SpriteData spriteData[MAX_SPRITES_PER_BATCH];
			SpriteVert spriteList[MAX_SPRITES_PER_BATCH*6];
		};

		Player* m_pPlayer;

		math::Vec3			m_Pos;

		int m_NumBatchesInUse;
		SpriteBatch m_Batches[MAX_BATCHES];

		bool m_AlphaBlend;
		float m_AlphaBlendValue;

		int m_DrawStyle;
		float m_MaxDistance;
};

#endif // __BATCHSPRITERENDER_H__
