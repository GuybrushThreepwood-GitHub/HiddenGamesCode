
/*===================================================================
	File: InsectAttack.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include <cmath>
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"

#include "H4.h"

#include "Resources/ResourceHelper.h"
#include "Resources/SoundResources.h"

#include "Audio/AudioSystem.h"

#include "GameEffects/InsectAttack.h"

namespace
{
	const int MAX_INSECTS = 25;
	const int MAX_ANIM_FRAMES = 5;
	const int MAX_FRAME_ORDERS = 4;
	const int VERTS_PER_INSECT = 12;
	const int SHAKE_DROP_COUNT = 3;
	const float DROP_TIME = 15.0f;
	const float FRAME_SWAP_TIME = 0.05f;

	const float SCALE_SPEED = 5.0f;
	const float COCKROACH_HW = 32.0f;
	const float COCKROACH_HH = 64.0f;

	const float ANT_HW = 22.5f;
	const float ANT_HH = 41.0f;

	struct InsectVert
	{
		math::Vec2 v;
		math::Vec2 uv;
	};

	struct InsectData
	{
		bool active;
		math::Vec2 pos;
		float rot;

		float animFrameTime;
		int currFrame;
	};

	int InsectAttackType = -1;
	bool StartedInsectAttack = false;
	GLuint InsectTex = renderer::INVALID_OBJECT;
	float DropTimer = 0.0f;
	int LastShakeCount = 0;

	int FrameOrders[MAX_FRAME_ORDERS] = { 0, 1, 2, 1 };

	InsectVert AntSprites[MAX_INSECTS*VERTS_PER_INSECT]; // 12 vert
	InsectData AntData[MAX_INSECTS];

	InsectVert CockroachSprites[MAX_INSECTS*VERTS_PER_INSECT]; // 12 vert
	InsectData CockroachData[MAX_INSECTS];

	math::Vec2 UVCockroach[MAX_ANIM_FRAMES][VERTS_PER_INSECT];
	math::Vec2 UVAnt[MAX_ANIM_FRAMES][VERTS_PER_INSECT];

	// audio
	snd::Sound SndLoad;
	ALuint m_AttackLoopAudio;
	ALuint m_AttackLoopSource;
	float AudioGain = 1.0f;

	float CockroachStart_HW = 1.0f;
	float CockroachStart_HH = 1.0f;

	float AntStart_HW = 1.0f;
	float AntStart_HH = 1.0f;

}

void SetupCockroachFrame( int spriteOffset, int frame );
void SetupAntFrame( int spriteOffset, int frame );

/////////////////////////////////////////////////////
/// Function: InitialiseInsectAttack
/// Params: None
///
/////////////////////////////////////////////////////
void InitialiseInsectAttack()
{
	int i=0;

	math::Vec2 testPos( 240.0f, 160.0f );
	InsectTex = res::LoadTextureName( "textures/insects", false, GL_NEAREST, GL_NEAREST );

	float cockroachFrameOffset = 0.25f;
	float antFrameOffset = 0.178f;

	float currentCockroachOffset = 0.0f;
	float currentAntOffset = 0.0f;

	// setup the anim frame uvs
	for( i=0; i < MAX_ANIM_FRAMES; ++i )
	{
		UVCockroach[i][0] = math::Vec2( 0.0f+currentCockroachOffset, 0.5f );
		UVCockroach[i][1] = math::Vec2( 0.25f+currentCockroachOffset, 0.5f );
		UVCockroach[i][2] = math::Vec2( 0.0f+currentCockroachOffset, 1.0f );
		UVCockroach[i][3] = math::Vec2( 0.25f+currentCockroachOffset, 0.5f );
		UVCockroach[i][4] = math::Vec2( 0.25f+currentCockroachOffset, 1.0f );
		UVCockroach[i][5] =	math::Vec2( 0.0f+currentCockroachOffset, 1.0f );

		UVCockroach[i][6] = math::Vec2( 0.25f+currentCockroachOffset, 0.5f );
		UVCockroach[i][7] = math::Vec2( 0.0f+currentCockroachOffset, 0.5f );
		UVCockroach[i][8] = math::Vec2( 0.25f+currentCockroachOffset, 1.0f );
		UVCockroach[i][9] = math::Vec2( 0.0f+currentCockroachOffset, 0.5f );
		UVCockroach[i][10] = math::Vec2( 0.0f+currentCockroachOffset, 1.0f );
		UVCockroach[i][11] = math::Vec2( 0.25f+currentCockroachOffset, 1.0f );

		const float antStart = 0.175f;

		UVAnt[i][0] = math::Vec2( 0.0f+currentAntOffset, 0.179f );
		UVAnt[i][1] = math::Vec2( antStart+currentAntOffset, 0.179f );
		UVAnt[i][2] = math::Vec2( 0.0f+currentAntOffset, 0.5f );
		UVAnt[i][3] = math::Vec2( antStart+currentAntOffset, 0.179f );
		UVAnt[i][4] = math::Vec2( antStart+currentAntOffset, 0.5f );
		UVAnt[i][5] = math::Vec2( 0.0f+currentAntOffset, 0.5f );

		UVAnt[i][6] = math::Vec2( antStart+currentAntOffset, 0.179f );
		UVAnt[i][7] = math::Vec2( 0.0f+currentAntOffset, 0.179f );
		UVAnt[i][8] = math::Vec2( antStart+currentAntOffset, 0.5f );
		UVAnt[i][9] = math::Vec2( 0.0f+currentAntOffset, 0.179f );
		UVAnt[i][10] = math::Vec2( 0.0f+currentAntOffset, 0.5f );
		UVAnt[i][11] = math::Vec2( antStart+currentAntOffset, 0.5f );

		currentCockroachOffset += cockroachFrameOffset;
		currentAntOffset += antFrameOffset;
	}

	const char* sr = 0;

	sr = res::GetSoundResource( 320 );
	m_AttackLoopAudio = snd::SoundLoad( sr, SndLoad );

	m_AttackLoopSource = snd::INVALID_SOUNDSOURCE;
}

/////////////////////////////////////////////////////
/// Function: CleanupInsectAttack
/// Params: None
///
/////////////////////////////////////////////////////
void CleanupInsectAttack()
{
	renderer::RemoveTexture( InsectTex );

	snd::RemoveSound( m_AttackLoopAudio );
}

/////////////////////////////////////////////////////
/// Function: StartInsectAttack
/// Params: None
///
/////////////////////////////////////////////////////
void StartInsectAttack( int insectAttack )
{
	CockroachStart_HW = 1.0f;
	CockroachStart_HH = 1.0f;

	AntStart_HW = 1.0f;
	AntStart_HH = 1.0f;

	int i=0;

	InsectAttackType = insectAttack;
	DropTimer = 0.0f;
	AudioGain = 1.0f;

	math::Vec3 zero( 0.0f, 0.0f, 0.0f );
	m_AttackLoopSource = AudioSystem::GetInstance()->PlayAudio( m_AttackLoopAudio, zero, true, true, 1.0f, AudioGain );

	for( i=0; i < MAX_INSECTS; ++i )
	{
		CockroachData[i].active = true;
		CockroachData[i].pos = math::Vec2( math::RandomNumber(50.0f, 430.0f), math::RandomNumber(50.0f, 270.0f) );
		CockroachData[i].rot = math::RandomNumber(-180.0f, 180.0f);
		CockroachData[i].currFrame = 0;
		CockroachData[i].animFrameTime = 0.0f;

		AntData[i].active = true;
		AntData[i].pos = math::Vec2( math::RandomNumber(50.0f, 430.0f), math::RandomNumber(50.0f, 270.0f) );
		AntData[i].rot = math::RandomNumber(-180.0f, 180.0f);
		AntData[i].currFrame = 0;
		AntData[i].animFrameTime = 0.0f;
	}

	// COCKROACHES
	for( i=0; i < MAX_INSECTS; ++i )
	{
		math::Vec2 pos = CockroachData[i].pos;

		CockroachSprites[i*VERTS_PER_INSECT].v = math::Vec2( pos.X - (CockroachStart_HW) + (-CockroachStart_HW), pos.Y + (-CockroachStart_HH) );
		CockroachSprites[i*VERTS_PER_INSECT+1].v = math::Vec2( pos.X - (CockroachStart_HW) + (CockroachStart_HW), pos.Y + (-CockroachStart_HH) );
		CockroachSprites[i*VERTS_PER_INSECT+2].v = math::Vec2( pos.X - (CockroachStart_HW) + (-CockroachStart_HW), pos.Y + (CockroachStart_HH) );
		CockroachSprites[i*VERTS_PER_INSECT+3].v = math::Vec2( pos.X - (CockroachStart_HW) + (CockroachStart_HW), pos.Y + (-CockroachStart_HH) );
		CockroachSprites[i*VERTS_PER_INSECT+4].v = math::Vec2( pos.X - (CockroachStart_HW) + (CockroachStart_HW), pos.Y + (CockroachStart_HH) );
		CockroachSprites[i*VERTS_PER_INSECT+5].v = math::Vec2( pos.X - (CockroachStart_HW) + (-CockroachStart_HW), pos.Y + (CockroachStart_HH) );

		// right side quad (mirrored)
		CockroachSprites[i*VERTS_PER_INSECT+6].v	= math::Vec2( pos.X + (CockroachStart_HW) + (-CockroachStart_HW) , pos.Y + (-CockroachStart_HH) );
		CockroachSprites[i*VERTS_PER_INSECT+7].v = math::Vec2( pos.X + (CockroachStart_HW) + (CockroachStart_HW), pos.Y + (-CockroachStart_HH) );
		CockroachSprites[i*VERTS_PER_INSECT+8].v = math::Vec2( pos.X + (CockroachStart_HW) + (-CockroachStart_HW), pos.Y + (CockroachStart_HH) );
		CockroachSprites[i*VERTS_PER_INSECT+9].v = math::Vec2( pos.X + (CockroachStart_HW) + (CockroachStart_HW), pos.Y + (-CockroachStart_HH) );
		CockroachSprites[i*VERTS_PER_INSECT+10].v = math::Vec2( pos.X + (CockroachStart_HW) + (CockroachStart_HW), pos.Y + (CockroachStart_HH) );
		CockroachSprites[i*VERTS_PER_INSECT+11].v = math::Vec2( pos.X + (CockroachStart_HW) + (-CockroachStart_HW), pos.Y + (CockroachStart_HH) );

		SetupCockroachFrame( i*VERTS_PER_INSECT, FrameOrders[CockroachData[i].currFrame] );
	}

	// ANTS

	for( i=0; i < MAX_INSECTS; ++i )
	{
		math::Vec2 pos = AntData[i].pos;

		// left side quad
		AntSprites[i*VERTS_PER_INSECT].v = math::Vec2( pos.X - (AntStart_HW) + (-AntStart_HW), pos.Y + (-AntStart_HH) );
		AntSprites[i*VERTS_PER_INSECT+1].v = math::Vec2( pos.X - (AntStart_HW) + (AntStart_HW), pos.Y + (-AntStart_HH) );
		AntSprites[i*VERTS_PER_INSECT+2].v = math::Vec2( pos.X - (AntStart_HW) + (-AntStart_HW), pos.Y + (AntStart_HH) );
		AntSprites[i*VERTS_PER_INSECT+3].v = math::Vec2( pos.X - (AntStart_HW) + (AntStart_HW), pos.Y + (-AntStart_HH) );
		AntSprites[i*VERTS_PER_INSECT+4].v = math::Vec2( pos.X - (AntStart_HW) + (AntStart_HW), pos.Y + (AntStart_HH) );
		AntSprites[i*VERTS_PER_INSECT+5].v = math::Vec2( pos.X - (AntStart_HW) + (-AntStart_HW), pos.Y + (AntStart_HH) );

		// right side quad (mirrored)
		AntSprites[i*VERTS_PER_INSECT+6].v	= math::Vec2( pos.X + (AntStart_HW) + (-AntStart_HW) , pos.Y + (-AntStart_HH) );
		AntSprites[i*VERTS_PER_INSECT+7].v = math::Vec2( pos.X + (AntStart_HW) + (AntStart_HW), pos.Y + (-AntStart_HH) );
		AntSprites[i*VERTS_PER_INSECT+8].v = math::Vec2( pos.X + (AntStart_HW) + (-AntStart_HW), pos.Y + (AntStart_HH) );
		AntSprites[i*VERTS_PER_INSECT+9].v = math::Vec2( pos.X + (AntStart_HW) + (AntStart_HW), pos.Y + (-AntStart_HH) );
		AntSprites[i*VERTS_PER_INSECT+10].v = math::Vec2( pos.X + (AntStart_HW) + (AntStart_HW), pos.Y + (AntStart_HH) );
		AntSprites[i*VERTS_PER_INSECT+11].v = math::Vec2( pos.X + (AntStart_HW) + (-AntStart_HW), pos.Y + (AntStart_HH) );

		SetupAntFrame( i*VERTS_PER_INSECT, FrameOrders[AntData[i].currFrame] );
	}

	StartedInsectAttack = true;
	LastShakeCount = 0;
}

/////////////////////////////////////////////////////
/// Function: EndInsectAttack
/// Params: None
///
/////////////////////////////////////////////////////
void EndInsectAttack( )
{
	InsectAttackType = -1;

	StartedInsectAttack = false;
	LastShakeCount = 0;

	if( m_AttackLoopSource != snd::INVALID_SOUNDSOURCE )
	{
		AudioSystem::GetInstance()->StopAudio( m_AttackLoopSource );
		m_AttackLoopSource = snd::INVALID_SOUNDSOURCE;
	}
}

/////////////////////////////////////////////////////
/// Function: DrawInsectAttack
/// Params: None
///
/////////////////////////////////////////////////////
void DrawInsectAttack()
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	glPushMatrix();
		glScalef( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );

		renderer::OpenGL::GetInstance()->EnableTextureArray();
		renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		renderer::OpenGL::GetInstance()->BindTexture( InsectTex );
	
		if( InsectAttackType == INSECTATTACK_ANTS )
		{
			glVertexPointer( 2, GL_FLOAT, sizeof(InsectVert), &AntSprites[0].v );
			glTexCoordPointer( 2, GL_FLOAT, sizeof(InsectVert), &AntSprites[0].uv );
		}
		else if( InsectAttackType == INSECTATTACK_COCKROACHES )
		{
			glVertexPointer( 2, GL_FLOAT, sizeof(InsectVert), &CockroachSprites[0].v );
			glTexCoordPointer( 2, GL_FLOAT, sizeof(InsectVert), &CockroachSprites[0].uv );
		}

		glDrawArrays( GL_TRIANGLES, 0, MAX_INSECTS*12 );

		renderer::OpenGL::GetInstance()->DisableTextureArray();
	glPopMatrix();
}

/////////////////////////////////////////////////////
/// Function: UpdateInsectAttack
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void UpdateInsectAttack( float deltaTime )
{
	int i=0;
	float sinVal;
	float cosVal;
	math::Vec2 vVector;
	math::Vec2 pos;
	float MOVE_SPEED = 40.0f;
	float DROP_SPEED = 200.0f;

	bool endAttack = true;

	if(!StartedInsectAttack)
		return;

	DropTimer += deltaTime;
	if( DropTimer >= DROP_TIME )
		DropTimer = DROP_TIME;

	for( i=0; i < MAX_INSECTS; ++i )
	{
		// COCKROACH
		CockroachStart_HW += SCALE_SPEED*deltaTime;
		CockroachStart_HH += SCALE_SPEED*deltaTime;
		if( CockroachStart_HW >= COCKROACH_HW )
			CockroachStart_HW = COCKROACH_HW;
		if( CockroachStart_HH >= COCKROACH_HH )
			CockroachStart_HH = COCKROACH_HH;

		if( InsectAttackType == INSECTATTACK_COCKROACHES )
		{
			if( DropTimer >= DROP_TIME )
			{
				CockroachData[i].active = false;
				AudioGain = 0.25f;

				if( m_AttackLoopSource != snd::INVALID_SOUNDSOURCE )
					alSourcef( m_AttackLoopSource, AL_GAIN, AudioGain );
			}

			if( CockroachData[i].active )
			{
				if( CockroachData[i].rot < 0.0f )
					CockroachData[i].rot -= 50.0f*deltaTime;
				else if( CockroachData[i].rot > 0.0f )
					CockroachData[i].rot += 50.0f*deltaTime;

				if( CockroachData[i].rot > 360.0f )
					CockroachData[i].rot -= 360.0f;
				if( CockroachData[i].rot < -360.0f )
					CockroachData[i].rot += 360.0f;

				CockroachData[i].animFrameTime += deltaTime;
				if(CockroachData[i].animFrameTime > FRAME_SWAP_TIME)
				{
					CockroachData[i].currFrame++;
					if( CockroachData[i].currFrame >= MAX_FRAME_ORDERS )
						CockroachData[i].currFrame = 0;

					SetupCockroachFrame( i*12, FrameOrders[CockroachData[i].currFrame] );
					CockroachData[i].animFrameTime = 0.0f;
				}

				pos = CockroachData[i].pos;
				math::sinCos( &sinVal, &cosVal, math::DegToRad(CockroachData[i].rot) ); 

				pos.X += sinVal*(MOVE_SPEED*deltaTime);
				pos.Y += cosVal*(MOVE_SPEED*deltaTime);

				endAttack = false;
			}
			else
			{
				pos = CockroachData[i].pos;

				math::sinCos( &sinVal, &cosVal, math::DegToRad(CockroachData[i].rot) ); 

				pos.Y -= (DROP_SPEED*deltaTime);
				if( pos.Y <= -100.0f )
					pos.Y = -100.0f;
				else
					endAttack = false;
			}

			CockroachData[i].pos = pos;

			CockroachSprites[i*VERTS_PER_INSECT].v = math::Vec2( pos.X - (CockroachStart_HW) + (-CockroachStart_HW), pos.Y + (-CockroachStart_HH) );
			vVector = CockroachSprites[i*VERTS_PER_INSECT].v  - pos;
			CockroachSprites[i*VERTS_PER_INSECT].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			CockroachSprites[i*VERTS_PER_INSECT].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);		
			
			CockroachSprites[i*VERTS_PER_INSECT+1].v = math::Vec2( pos.X - (CockroachStart_HW) + (CockroachStart_HW), pos.Y + (-CockroachStart_HH) );
			vVector = CockroachSprites[i*VERTS_PER_INSECT+1].v  - pos;
			CockroachSprites[i*VERTS_PER_INSECT+1].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			CockroachSprites[i*VERTS_PER_INSECT+1].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			CockroachSprites[i*VERTS_PER_INSECT+2].v = math::Vec2( pos.X - (CockroachStart_HW) + (-CockroachStart_HW), pos.Y + (CockroachStart_HH) );
			vVector = CockroachSprites[i*VERTS_PER_INSECT+2].v  - pos;
			CockroachSprites[i*VERTS_PER_INSECT+2].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			CockroachSprites[i*VERTS_PER_INSECT+2].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			CockroachSprites[i*VERTS_PER_INSECT+3].v = math::Vec2( pos.X - (CockroachStart_HW) + (CockroachStart_HW), pos.Y + (-CockroachStart_HH) );
			vVector = CockroachSprites[i*VERTS_PER_INSECT+3].v  - pos;
			CockroachSprites[i*VERTS_PER_INSECT+3].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			CockroachSprites[i*VERTS_PER_INSECT+3].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			CockroachSprites[i*VERTS_PER_INSECT+4].v = math::Vec2( pos.X - (CockroachStart_HW) + (CockroachStart_HW), pos.Y + (CockroachStart_HH) );
			vVector = CockroachSprites[i*VERTS_PER_INSECT+4].v  - pos;
			CockroachSprites[i*VERTS_PER_INSECT+4].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			CockroachSprites[i*VERTS_PER_INSECT+4].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			CockroachSprites[i*VERTS_PER_INSECT+5].v = math::Vec2( pos.X - (CockroachStart_HW) + (-CockroachStart_HW), pos.Y + (CockroachStart_HH) );
			vVector = CockroachSprites[i*VERTS_PER_INSECT+5].v  - pos;
			CockroachSprites[i*VERTS_PER_INSECT+5].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			CockroachSprites[i*VERTS_PER_INSECT+5].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			// right side quad (mirrored)
			CockroachSprites[i*VERTS_PER_INSECT+6].v = math::Vec2( pos.X + (CockroachStart_HW) + (-CockroachStart_HW) , pos.Y + (-CockroachStart_HH) );
			vVector = CockroachSprites[i*VERTS_PER_INSECT+6].v  - pos;
			CockroachSprites[i*VERTS_PER_INSECT+6].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			CockroachSprites[i*VERTS_PER_INSECT+6].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			CockroachSprites[i*VERTS_PER_INSECT+7].v = math::Vec2( pos.X + (CockroachStart_HW) + (CockroachStart_HW), pos.Y + (-CockroachStart_HH) );
			vVector = CockroachSprites[i*VERTS_PER_INSECT+7].v  - pos;
			CockroachSprites[i*VERTS_PER_INSECT+7].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			CockroachSprites[i*VERTS_PER_INSECT+7].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			CockroachSprites[i*VERTS_PER_INSECT+8].v = math::Vec2( pos.X + (CockroachStart_HW) + (-CockroachStart_HW), pos.Y + (CockroachStart_HH) );
			vVector = CockroachSprites[i*VERTS_PER_INSECT+8].v  - pos;
			CockroachSprites[i*VERTS_PER_INSECT+8].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			CockroachSprites[i*VERTS_PER_INSECT+8].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			CockroachSprites[i*VERTS_PER_INSECT+9].v = math::Vec2( pos.X + (CockroachStart_HW) + (CockroachStart_HW), pos.Y + (-CockroachStart_HH) );
			vVector = CockroachSprites[i*VERTS_PER_INSECT+9].v  - pos;
			CockroachSprites[i*VERTS_PER_INSECT+9].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			CockroachSprites[i*VERTS_PER_INSECT+9].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			CockroachSprites[i*VERTS_PER_INSECT+10].v = math::Vec2( pos.X + (CockroachStart_HW) + (CockroachStart_HW), pos.Y + (CockroachStart_HH) );
			vVector = CockroachSprites[i*VERTS_PER_INSECT+10].v  - pos;
			CockroachSprites[i*VERTS_PER_INSECT+10].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			CockroachSprites[i*VERTS_PER_INSECT+10].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			CockroachSprites[i*VERTS_PER_INSECT+11].v = math::Vec2( pos.X + (CockroachStart_HW) + (-CockroachStart_HW), pos.Y + (CockroachStart_HH) );
			vVector = CockroachSprites[i*VERTS_PER_INSECT+11].v  - pos;
			CockroachSprites[i*VERTS_PER_INSECT+11].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			CockroachSprites[i*VERTS_PER_INSECT+11].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	
		}

		// ANTS 
		AntStart_HW += SCALE_SPEED*deltaTime;
		AntStart_HH += SCALE_SPEED*deltaTime;
		if( AntStart_HW >= ANT_HW )
			AntStart_HW = ANT_HW;
		if( AntStart_HH >= ANT_HH )
			AntStart_HH = ANT_HH;

		if( InsectAttackType == INSECTATTACK_ANTS )
		{
			if( DropTimer >= DROP_TIME )
			{
				AntData[i].active = false;
				AudioGain = 0.25f;

				if( m_AttackLoopSource != snd::INVALID_SOUNDSOURCE )
					alSourcef( m_AttackLoopSource, AL_GAIN, AudioGain );
			}

			if( AntData[i].active )
			{
				if( AntData[i].rot < 0.0f )
					AntData[i].rot -= 50.0f*deltaTime;
				else if( AntData[i].rot > 0.0f )
					AntData[i].rot += 50.0f*deltaTime;

				if( AntData[i].rot > 360.0f )
					AntData[i].rot -= 360.0f;
				if( AntData[i].rot < -360.0f )
					AntData[i].rot += 360.0f;

				AntData[i].animFrameTime += deltaTime;
				if(AntData[i].animFrameTime > FRAME_SWAP_TIME)
				{
					AntData[i].currFrame++;
					if( AntData[i].currFrame >= MAX_FRAME_ORDERS )
						AntData[i].currFrame = 0;

					SetupAntFrame( i*12, FrameOrders[AntData[i].currFrame] );
					AntData[i].animFrameTime = 0.0f;
				}

				pos = AntData[i].pos;
				math::sinCos( &sinVal, &cosVal, math::DegToRad(AntData[i].rot) ); 

				pos.X += sinVal*(MOVE_SPEED*deltaTime);
				pos.Y += cosVal*(MOVE_SPEED*deltaTime);

				endAttack = false;
			}
			else
			{
				pos = AntData[i].pos;

				math::sinCos( &sinVal, &cosVal, math::DegToRad(AntData[i].rot) ); 
				pos.Y -= (DROP_SPEED*deltaTime);
				if( pos.Y <= -100.0f )
					pos.Y = -100.0f;
				else
					endAttack = false;
			}

			AntData[i].pos = pos;

			AntSprites[i*VERTS_PER_INSECT].v = math::Vec2( pos.X - (AntStart_HW) + (-AntStart_HW), pos.Y + (-AntStart_HH) );
			vVector = AntSprites[i*VERTS_PER_INSECT].v  - pos;
			AntSprites[i*VERTS_PER_INSECT].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			AntSprites[i*VERTS_PER_INSECT].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);		
			
			AntSprites[i*VERTS_PER_INSECT+1].v = math::Vec2( pos.X - (AntStart_HW) + (AntStart_HW), pos.Y + (-AntStart_HH) );
			vVector = AntSprites[i*VERTS_PER_INSECT+1].v  - pos;
			AntSprites[i*VERTS_PER_INSECT+1].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			AntSprites[i*VERTS_PER_INSECT+1].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			AntSprites[i*VERTS_PER_INSECT+2].v = math::Vec2( pos.X - (AntStart_HW) + (-AntStart_HW), pos.Y + (AntStart_HH) );
			vVector = AntSprites[i*VERTS_PER_INSECT+2].v  - pos;
			AntSprites[i*VERTS_PER_INSECT+2].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			AntSprites[i*VERTS_PER_INSECT+2].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			AntSprites[i*VERTS_PER_INSECT+3].v = math::Vec2( pos.X - (AntStart_HW) + (AntStart_HW), pos.Y + (-AntStart_HH) );
			vVector = AntSprites[i*VERTS_PER_INSECT+3].v  - pos;
			AntSprites[i*VERTS_PER_INSECT+3].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			AntSprites[i*VERTS_PER_INSECT+3].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			AntSprites[i*VERTS_PER_INSECT+4].v = math::Vec2( pos.X - (AntStart_HW) + (AntStart_HW), pos.Y + (AntStart_HH) );
			vVector = AntSprites[i*VERTS_PER_INSECT+4].v  - pos;
			AntSprites[i*VERTS_PER_INSECT+4].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			AntSprites[i*VERTS_PER_INSECT+4].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			AntSprites[i*VERTS_PER_INSECT+5].v = math::Vec2( pos.X - (AntStart_HW) + (-AntStart_HW), pos.Y + (AntStart_HH) );
			vVector = AntSprites[i*VERTS_PER_INSECT+5].v  - pos;
			AntSprites[i*VERTS_PER_INSECT+5].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			AntSprites[i*VERTS_PER_INSECT+5].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			// right side quad (mirrored)
			AntSprites[i*VERTS_PER_INSECT+6].v = math::Vec2( pos.X + (AntStart_HW) + (-AntStart_HW) , pos.Y + (-AntStart_HH) );
			vVector = AntSprites[i*VERTS_PER_INSECT+6].v  - pos;
			AntSprites[i*VERTS_PER_INSECT+6].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			AntSprites[i*VERTS_PER_INSECT+6].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			AntSprites[i*VERTS_PER_INSECT+7].v = math::Vec2( pos.X + (AntStart_HW) + (AntStart_HW), pos.Y + (-AntStart_HH) );
			vVector = AntSprites[i*VERTS_PER_INSECT+7].v  - pos;
			AntSprites[i*VERTS_PER_INSECT+7].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			AntSprites[i*VERTS_PER_INSECT+7].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			AntSprites[i*VERTS_PER_INSECT+8].v = math::Vec2( pos.X + (AntStart_HW) + (-AntStart_HW), pos.Y + (AntStart_HH) );
			vVector = AntSprites[i*VERTS_PER_INSECT+8].v  - pos;
			AntSprites[i*VERTS_PER_INSECT+8].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			AntSprites[i*VERTS_PER_INSECT+8].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			AntSprites[i*VERTS_PER_INSECT+9].v = math::Vec2( pos.X + (AntStart_HW) + (AntStart_HW), pos.Y + (-AntStart_HH) );
			vVector = AntSprites[i*VERTS_PER_INSECT+9].v  - pos;
			AntSprites[i*VERTS_PER_INSECT+9].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			AntSprites[i*VERTS_PER_INSECT+9].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			AntSprites[i*VERTS_PER_INSECT+10].v = math::Vec2( pos.X + (AntStart_HW) + (AntStart_HW), pos.Y + (AntStart_HH) );
			vVector = AntSprites[i*VERTS_PER_INSECT+10].v  - pos;
			AntSprites[i*VERTS_PER_INSECT+10].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			AntSprites[i*VERTS_PER_INSECT+10].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);	

			AntSprites[i*VERTS_PER_INSECT+11].v = math::Vec2( pos.X + (AntStart_HW) + (-AntStart_HW), pos.Y + (AntStart_HH) );
			vVector = AntSprites[i*VERTS_PER_INSECT+11].v  - pos;
			AntSprites[i*VERTS_PER_INSECT+11].v.X = (pos.X + sinVal*vVector.Y + cosVal*vVector.X);
			AntSprites[i*VERTS_PER_INSECT+11].v.Y = (pos.Y + cosVal*vVector.Y - sinVal*vVector.X);
		}
	}

	if( endAttack )
	{
		EndInsectAttack();
	}
}

/////////////////////////////////////////////////////
/// Function: SetupCockroachFrame
/// Params: [in]spriteOffset, [in]frame
///
/////////////////////////////////////////////////////
void SetupCockroachFrame( int spriteOffset, int frame )
{
	int correctFrame = FrameOrders[frame];

	CockroachSprites[spriteOffset].uv = UVCockroach[correctFrame][0];
	CockroachSprites[spriteOffset+1].uv = UVCockroach[correctFrame][1];
	CockroachSprites[spriteOffset+2].uv = UVCockroach[correctFrame][2];
	CockroachSprites[spriteOffset+3].uv = UVCockroach[correctFrame][3];
	CockroachSprites[spriteOffset+4].uv = UVCockroach[correctFrame][4];
	CockroachSprites[spriteOffset+5].uv = UVCockroach[correctFrame][5];

	int rightFrame = frame+2;
	
	if( rightFrame >= MAX_FRAME_ORDERS )
		rightFrame -= MAX_FRAME_ORDERS;

	correctFrame = FrameOrders[rightFrame]; 

	CockroachSprites[spriteOffset+6].uv = UVCockroach[correctFrame][6];
	CockroachSprites[spriteOffset+7].uv = UVCockroach[correctFrame][7];
	CockroachSprites[spriteOffset+8].uv = UVCockroach[correctFrame][8];
	CockroachSprites[spriteOffset+9].uv = UVCockroach[correctFrame][9];
	CockroachSprites[spriteOffset+10].uv = UVCockroach[correctFrame][10];
	CockroachSprites[spriteOffset+11].uv = UVCockroach[correctFrame][11];
}

/////////////////////////////////////////////////////
/// Function: SetupAntFrame
/// Params: [in]spriteOffset, [in]frame
///
/////////////////////////////////////////////////////
void SetupAntFrame( int spriteOffset, int frame )
{
	int correctFrame = FrameOrders[frame];

	AntSprites[spriteOffset].uv = UVAnt[correctFrame][0];
	AntSprites[spriteOffset+1].uv = UVAnt[correctFrame][1];
	AntSprites[spriteOffset+2].uv = UVAnt[correctFrame][2];
	AntSprites[spriteOffset+3].uv = UVAnt[correctFrame][3];
	AntSprites[spriteOffset+4].uv = UVAnt[correctFrame][4];
	AntSprites[spriteOffset+5].uv = UVAnt[correctFrame][5];

	int rightFrame = frame+2;
	
	if( rightFrame >= MAX_FRAME_ORDERS )
		rightFrame -= MAX_FRAME_ORDERS;

	correctFrame = FrameOrders[rightFrame]; 

	AntSprites[spriteOffset+6].uv = UVAnt[correctFrame][6];
	AntSprites[spriteOffset+7].uv = UVAnt[correctFrame][7];
	AntSprites[spriteOffset+8].uv = UVAnt[correctFrame][8];
	AntSprites[spriteOffset+9].uv = UVAnt[correctFrame][9];
	AntSprites[spriteOffset+10].uv = UVAnt[correctFrame][10];
	AntSprites[spriteOffset+11].uv = UVAnt[correctFrame][11];
}

/////////////////////////////////////////////////////
/// Function: ApplyShake
/// Params: None
///
/////////////////////////////////////////////////////
void ApplyShake()
{
	int i=0;

	if( LastShakeCount < MAX_INSECTS )
	{
		for( i=LastShakeCount; i < LastShakeCount+SHAKE_DROP_COUNT; ++i )
		{
			if( CockroachData[i].active )
				CockroachData[i].active = false;

			if( AntData[i].active )
				AntData[i].active = false;
		}

		LastShakeCount += SHAKE_DROP_COUNT;

		AudioGain -= 0.05f;
		if( AudioGain <= 0.0f )
			AudioGain = 0.0f;

		if( m_AttackLoopSource != snd::INVALID_SOUNDSOURCE )
			alSourcef( m_AttackLoopSource, AL_GAIN, AudioGain );
	}
}

/////////////////////////////////////////////////////
/// Function: IsInsectAttackActive
/// Params: None
///
/////////////////////////////////////////////////////
bool IsInsectAttackActive()
{
	return(StartedInsectAttack);
}