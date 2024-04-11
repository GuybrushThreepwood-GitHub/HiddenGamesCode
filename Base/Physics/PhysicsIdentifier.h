
/*===================================================================
	File: PhysicsIdentifier.h
	Library: Physics

	(C)Hidden Games
=====================================================================*/

#ifndef __PHYSICSIDENTIFIER_H__
#define __PHYSICSIDENTIFIER_H__

namespace physics
{
	class PhysicsIdentifier
	{
		public:
			PhysicsIdentifier()
			{
				m_Identifier = -1;
				m_CastIdentifier = -1;

				m_Numeric1 = -9999;
				m_Numeric2 = -9999;
				m_Numeric3 = -9999;
				m_Numeric4 = -9999;
				m_Numeric5 = -9999;

				m_Data = 0;
			}
			PhysicsIdentifier( int identifier, int castingId )
			{
				m_Identifier = identifier;
				m_CastIdentifier = castingId;

				m_Numeric1 = -9999;
				m_Numeric2 = -9999;
				m_Numeric3 = -9999;
				m_Numeric4 = -9999;
				m_Numeric5 = -9999;

				m_Data = 0;
			}

			virtual ~PhysicsIdentifier()
			{
			}

			void SetBaseId( int identifier )		{ m_Identifier = identifier; }
			int GetBaseId()							{ return m_Identifier; }

			void SetCastingId( int castingId )		{ m_CastIdentifier = castingId; }
			int GetCastingId()						{ return m_CastIdentifier; }

			void SetNumeric1( int val ) { m_Numeric1 = val; }
			int GetNumeric1()			{ return m_Numeric1; }
			void SetNumeric2( int val ) { m_Numeric2 = val; }
			int GetNumeric2()			{ return m_Numeric2; }
			void SetNumeric3( int val ) { m_Numeric3 = val; }
			int GetNumeric3()			{ return m_Numeric3; }
			void SetNumeric4( int val ) { m_Numeric4 = val; }
			int GetNumeric4()			{ return m_Numeric4; }
			void SetNumeric5( int val ) { m_Numeric5 = val; }
			int GetNumeric5()			{ return m_Numeric5; }

			void SetData( void* val )	{ m_Data = val; }
			void* GetData()				{ return m_Data; }

		protected:
			int m_Identifier;
			int m_CastIdentifier;

			int m_Numeric1;
			int m_Numeric2;
			int m_Numeric3;
			int m_Numeric4;
			int m_Numeric5;

			void* m_Data;

		private:

	};
} // namespace physics

#endif // __PHYSICSIDENTIFIER_H__
