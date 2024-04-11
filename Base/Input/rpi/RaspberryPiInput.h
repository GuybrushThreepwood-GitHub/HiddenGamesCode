
#ifndef __RASPBERRYPIINPUT_H__
#define __RASPBERRYPIINPUT_H__

namespace input
{
#ifdef BASE_PLATFORM_RASPBERRYPI

	// these should match linux/input.h
	const int KEY_ESCAPE	= 1;
	const int KEY_PAGEUP	= 104;
	const int KEY_PAGEDOWN	= 109;

	const int KEY_0			= 11;
	const int KEY_1			= 2;
	const int KEY_2			= 3;
	const int KEY_3			= 4;
	const int KEY_4			= 5;
	const int KEY_5			= 6;
	const int KEY_6			= 7;
	const int KEY_7			= 8;
	const int KEY_8			= 9;
	const int KEY_9			= 10;	

	const int KEY_A			= 30;
	const int KEY_B			= 48;
	const int KEY_C			= 46;
	const int KEY_D			= 32;
	const int KEY_E			= 18;
	const int KEY_F			= 33;
	const int KEY_G			= 34;
	const int KEY_H			= 35;
	const int KEY_I			= 23;
	const int KEY_J			= 36;
	const int KEY_K			= 37;
	const int KEY_L			= 38;
	const int KEY_M			= 50;
	const int KEY_N			= 49;
	const int KEY_O			= 24;
	const int KEY_P			= 25;
	const int KEY_Q			= 16;
	const int KEY_R			= 19;
	const int KEY_S			= 31;
	const int KEY_T			= 20;
	const int KEY_U			= 22;
	const int KEY_V			= 47;
	const int KEY_W			= 17;
	const int KEY_X			= 45;
	const int KEY_Y			= 21;
	const int KEY_Z			= 44;

	const int KEY_LBRACKET		= 26; // [
	const int KEY_RBRACKET		= 27; // ]

	const int KEY_COMMA			= 51; // ,
	const int KEY_PERIOD		= 52; // .

	const int KEY_PLUS			= 13;
	const int KEY_MINUS			= 12;

	const int KEY_F1			= 59;
	const int KEY_F2			= 60;
	const int KEY_F3			= 61;
	const int KEY_F4			= 62;
	const int KEY_F5			= 63;
	const int KEY_F6			= 64;
	const int KEY_F7			= 65;
	const int KEY_F8			= 66;
	const int KEY_F9			= 67;
	const int KEY_F10			= 68;

	const int KEY_BACKSPACE		= 14;
	const int KEY_UPARROW		= 103;
	const int KEY_DOWNARROW		= 108;
	const int KEY_RIGHTARROW	= 106;
	const int KEY_LEFTARROW		= 105;

	const int KEY_TAB			= 15;
	const int KEY_ENTER			= 28;
	const int KEY_SPACE			= 57;

	const int KEY_LSHIFT		= 42;
	const int KEY_RSHIFT		= 54;
	const int KEY_LCONTROL		= 29; 
	const int KEY_RCONTROL		= 97;
	const int KEY_LALT			= 56;
	const int KEY_RALT			= 100;
#endif // BASE_PLATFORM_RASPBERRYPI

} // namespace input

#endif // __RASPBERRYPIINPUT_H__

