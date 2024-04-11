

#ifndef __WININPUT_H__
#define __WININPUT_H__

namespace input
{
#ifdef BASE_PLATFORM_WINDOWS

	const int KEY_ESCAPE	= VK_ESCAPE;
	const int KEY_PAGEUP	= VK_PRIOR;
	const int KEY_PAGEDOWN	= VK_NEXT;
	const int KEY_0			= 48;
	const int KEY_1			= 49;
	const int KEY_2			= 50;
	const int KEY_3			= 51;
	const int KEY_4			= 52;
	const int KEY_5			= 53;
	const int KEY_6			= 54;
	const int KEY_7			= 55;
	const int KEY_8			= 56;
	const int KEY_9			= 57;	

	const int KEY_A			= 65;
	const int KEY_B			= 66;
	const int KEY_C			= 67;
	const int KEY_D			= 68;
	const int KEY_E			= 69;
	const int KEY_F			= 70;
	const int KEY_G			= 71;
	const int KEY_H			= 72;
	const int KEY_I			= 73;
	const int KEY_J			= 74;
	const int KEY_K			= 75;
	const int KEY_L			= 76;
	const int KEY_M			= 77;
	const int KEY_N			= 78;
	const int KEY_O			= 79;
	const int KEY_P			= 80;
	const int KEY_Q			= 81;
	const int KEY_R			= 82;
	const int KEY_S			= 83;
	const int KEY_T			= 84;
	const int KEY_U			= 85;
	const int KEY_V			= 86;
	const int KEY_W			= 87;
	const int KEY_X			= 88;
	const int KEY_Y			= 89;
	const int KEY_Z			= 90;

	const int KEY_LBRACKET		= 219; // [
	const int KEY_RBRACKET		= 221; // ]
	
	const int KEY_COMMA			= 188; // ,
	const int KEY_PERIOD		= 190; // .

	const int KEY_PLUS			= VK_ADD;
	const int KEY_MINUS			= VK_SUBTRACT;


	const int KEY_F1			= VK_F1;
	const int KEY_F2			= VK_F2;
	const int KEY_F3			= VK_F3;
	const int KEY_F4			= VK_F4;
	const int KEY_F5			= VK_F5;
	const int KEY_F6			= VK_F6;
	const int KEY_F7			= VK_F7;
	const int KEY_F8			= VK_F8;
	const int KEY_F9			= VK_F9;
	const int KEY_F10			= VK_F10;

	const int KEY_BACKSPACE		= VK_BACK;
	const int KEY_UPARROW		= VK_UP;
	const int KEY_DOWNARROW		= VK_DOWN;
	const int KEY_RIGHTARROW	= VK_RIGHT;
	const int KEY_LEFTARROW		= VK_LEFT;

	const int KEY_TAB			= VK_TAB;
	const int KEY_ENTER			= VK_RETURN;
	const int KEY_SPACE			= VK_SPACE;

	const int KEY_LSHIFT		= VK_SHIFT;
	const int KEY_RSHIFT		= VK_SHIFT;
	const int KEY_LCONTROL		= VK_LCONTROL; 
	const int KEY_RCONTROL		= VK_RCONTROL;
	const int KEY_LALT			= VK_MENU;
	const int KEY_RALT			= VK_MENU;
#endif // BASE_PLATFORM_WINDOWS

} // namespace input

#endif // __WININPUT_H__

