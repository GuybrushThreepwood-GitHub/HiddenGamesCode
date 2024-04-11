
#ifndef __MICROGAME_H__
#define __MICROGAME_H__

class MicroGame
{
	public:
		MicroGame() 
		{
			m_Complete = false; 
			m_ReturnCode = false;
		}
		virtual ~MicroGame() 
		{
#if defined( BASE_PLATFORM_iOS ) || defined( BASE_PLATFORM_ANDROID )
			input::DirtyTouchTicks();
#endif // BASE_PLATFORM_iOS
		}

		virtual void Initialise() = 0;
		virtual void Release() = 0;

		virtual void OnEnter() = 0;
		virtual void OnExit() = 0;

		virtual void Update( float deltaTime ) = 0;
		virtual void Draw() = 0;

		virtual bool IsComplete() = 0;
		virtual bool ReturnCode() = 0;

	protected:
		bool m_Complete;
		bool m_ReturnCode;

	private:
};

#endif // __MICROGAME_H__
