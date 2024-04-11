
#ifndef __CONSOLEPRINT_H__
#define __CONSOLEPRINT_H__

class ConsolePrint
{
	public:
		/// default constructor
		ConsolePrint();
		/// default destructor
		~ConsolePrint();

		/// Draw - Draws the active wave grid
		void Draw( void );
		/// DrawDebug - Draws debug data about the wave
		void DrawDebug( void );
		/// Update - Updates the wave grid motion
		/// \param deltaTime - time delta from previous frame
		void Update( float deltaTime );

	private:

};

#endif // __CONSOLEPRINT_H__
