
#ifndef __WAVEGRID_H__
#define __WAVEGRID_H__

class WaveGrid
{
	public:
		/// default constructor
		WaveGrid( const math::Vec3& pos, int gridWidth, int gridDepth, int gridWidthPoints, int gridDepthPoints, float sinAngle, float waveTime );
		/// default destructor
		~WaveGrid();

		/// Draw - Draws the active wave grid
		void Draw( void );
		/// DrawDebug - Draws debug data about the wave
		void DrawDebug( void );
		/// Update - Updates the wave grid motion
		/// \param deltaTime - time delta from previous frame
		void Update( float deltaTime );

		/// GetAABB - get the axis aligned bound box
		/// \return collision::AABB - the bounding box
		collision::AABB GetAABB()		{ return m_AABB; }

	private:
		/// SetupVertexBufferObject - Generates the vertex buffer objects GL ID's
		void SetupVertexBufferObject( void );
		/// DrawVertexBufferObject - Draws the mesh using vertex buffer objects
		void DrawVertexBufferObject( void );
		/// DrawVA - Draws the mesh using vertex arrays
		void DrawVA( void );

	private:
		math::Vec3			m_Pos;
		collision::AABB		m_AABB;
		math::Vec3**		m_AnimatedVertices;
		math::Vec3*			m_Vertices;
		GLfloat*			m_TexCoords;
		GLuint				m_StripVertexCount;
		float				m_WaveTime;
		float				m_WaveChangeTime;

		float				m_SinAngle;
		int					m_GridWidth;
		int					m_GridDepth;
		int					m_GridWidthPoints;
		int					m_GridDepthPoints;
		float				m_WidthPointOffset;
		float				m_DepthPointOffset;

		/// VBO initialised flag
		bool bVBOInitialised;
		/// GL VBO vertex object id for this model
		GLuint nVertexBufferObject;
		/// GL VBO 2D texture object id for this model
		GLuint nTextureBufferObject2D;
		/// GL VBO normal object id for this model
		GLuint nNormalBufferObject;
};

#endif // __WAVEGRID_H__
