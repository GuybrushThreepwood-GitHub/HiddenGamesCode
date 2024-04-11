
/*===================================================================
	File: Bullet.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __BULLET_H__
#define __BULLET_H__

class Bullet : public physics::PhysicsIdentifier
{
	public:
		enum BulletSource
		{
			BULLET_PISTOL,
			BULLET_SHOTGUN
		};

	public:
		Bullet();
		~Bullet();

		void Update( float deltaTime );
		void Draw();

		void DebugDraw();

		void HandleContact( int idNum, int castId, const ContactPoint* contact, const b2Fixture* pOtherFixture, const b2Body* pOtherBody );

		void Spawn( BulletSource src, const math::Vec3& spawnPos, float rot, const math::Vec3& impulse );

		void Clear();

		void SetActive( bool state )					{ m_Active = state;		}
		bool IsActive()	const							{ return m_Active;		}
		b2Body* GetBody()								{ return m_Body;		}
		const collision::Sphere& GetCollision() const	{ return m_Collision;	}
		BulletSource GetBulletSource()					{ return m_Source;		}

	private:
		BulletSource m_Source;

		bool m_Active;
		b2Body* m_Body;
		b2Fixture* m_Fixture;

		snd::Sound m_SndLoad;

		float m_YPos;
		float m_YStartPos;
		float m_YImpulse;

		math::Vec3 m_Pos;
		collision::Sphere m_Collision;
		float m_Rot;
};

#endif // __BULLET_H__

