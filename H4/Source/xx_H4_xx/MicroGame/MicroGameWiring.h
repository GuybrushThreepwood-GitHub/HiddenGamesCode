
#ifndef __MICROGAMEWIRING_H__
#define __MICROGAMEWIRING_H__

#include "MicroGame.h"
#include "GameStates/UI/UIFileLoader.h"

const int MAX_WIRES = 6;

class MicroGameWiring : public MicroGame
{
	public:
		MicroGameWiring( int wireIndex1, int wireIndex2 );
		virtual ~MicroGameWiring();

		void Initialise();
		void Release();

		void OnEnter();
		void OnExit();

		void Update( float deltaTime );
		void Draw();

		bool IsComplete()		{ return m_Complete; }
		bool ReturnCode()		{ return m_ReturnCode; }

	private:
		mdl::ModelHGM* m_WiringModel;
		UIFileLoader m_WiringUI;

		ALuint m_SparkAudio;
		ALuint m_SuccessAudio;
		bool m_CorrectWires;

		int m_CorrectWire1;
		int m_CorrectWire2;

		int m_FirstWireIndex;
		int m_SecondWireIndex;

		bool m_SelectingFirstWire;

		float m_ResetTimer;

		struct WireSetup
		{
			math::Vec3 startPos;

			math::Vec3 minLimits;
			math::Vec3 maxLimits;
		};

		WireSetup m_WireList[MAX_WIRES];
};

#endif // __MICROGAMEWIRING_H__
