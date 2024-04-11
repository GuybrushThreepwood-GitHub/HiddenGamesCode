
/*===================================================================
	File: TextFormattingFuncs.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Input/Input.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "CabbyConsts.h"
#include "Cabby.h"

#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/UI/TextFormattingFuncs.h"

namespace
{
	math::Vec3 vaPoints[4];
	math::Vec4Lite vaColours[4];
}

/////////////////////////////////////////////////////
/// Function: SetCashStringNoSymbol
/// Params:
///
/////////////////////////////////////////////////////
void SetCashStringNoSymbol( UIFileLoader& uiFile, int elementId, int count )
{
	const int BUFFER_SIZE = 16;
	char finalString[BUFFER_SIZE];
	bool lessThanZero = false;

	if( count < 0 )
	{
		lessThanZero = true;
		count = -count;
	}

	if( count > 999999 )
		count = 999999;

	if( count >= 1000 )
	{
		int exp = count / 1000;
		int man = count % 1000;

		if( man == 0 )
		{
			if( lessThanZero )
				snprintf( finalString, BUFFER_SIZE, "-%d,000", exp );
			else
				snprintf( finalString, BUFFER_SIZE, "%d,000", exp );
		}
		else
		{
			if( man < 10 && count >= 100 )
			{
				if( lessThanZero )
					snprintf( finalString, BUFFER_SIZE, "-%d,00%d", exp, man );
				else
					snprintf( finalString, BUFFER_SIZE, "%d,00%d", exp, man );
			}
			else if( man < 100 && count >= 1000 )
			{
				if( lessThanZero )
					snprintf( finalString, BUFFER_SIZE, "-%d,0%d", exp, man );
				else
					snprintf( finalString, BUFFER_SIZE, "%d,0%d", exp, man );
			}
			else
			{
				if( lessThanZero )
					snprintf( finalString, BUFFER_SIZE, "-%d,%d", exp, man );				
				else
					snprintf( finalString, BUFFER_SIZE, "%d,%d", exp, man );
			}
		}
	}
	else
	{
		if( lessThanZero )
			snprintf( finalString, BUFFER_SIZE, "-%d", count );
		else
			snprintf( finalString, BUFFER_SIZE, "%d", count );
	}

	uiFile.ChangeElementText( elementId, "%s", finalString );
}

/////////////////////////////////////////////////////
/// Function: SetCashString
/// Params:
///
/////////////////////////////////////////////////////
void SetCashString( UIFileLoader& uiFile, int elementId, int count )
{
	const int BUFFER_SIZE = 16;
	char finalString[BUFFER_SIZE];
	bool lessThanZero = false;

	if( count < 0 )
	{
		lessThanZero = true;
		count = -count;
	}

	if( count > 999999 )
		count = 999999;

	if( count >= 1000 )
	{
		int exp = count / 1000;
		int man = count % 1000;

		if( man == 0 )
		{
			if( lessThanZero )
				snprintf( finalString, BUFFER_SIZE, "-%d,000", exp );
			else
				snprintf( finalString, BUFFER_SIZE, "%d,000", exp );
		}
		else
		{
			if( man < 10 && count >= 100 )
			{
				if( lessThanZero )
					snprintf( finalString, BUFFER_SIZE, "-%d,00%d", exp, man );
				else
					snprintf( finalString, BUFFER_SIZE, "%d,00%d", exp, man );
			}
			else if( man < 100 && count >= 1000 )
			{
				if( lessThanZero )
					snprintf( finalString, BUFFER_SIZE, "-%d,0%d", exp, man );
				else
					snprintf( finalString, BUFFER_SIZE, "%d,0%d", exp, man );
			}
			else
			{
				if( lessThanZero )
					snprintf( finalString, BUFFER_SIZE, "-%d,%d", exp, man );				
				else
					snprintf( finalString, BUFFER_SIZE, "%d,%d", exp, man );
			}
		}
	}
	else
	{
		if( lessThanZero )
			snprintf( finalString, BUFFER_SIZE, "-%d", count );
		else
			snprintf( finalString, BUFFER_SIZE, "%d", count );
	}

	uiFile.ChangeElementText( elementId, "%s", finalString );
}

/////////////////////////////////////////////////////
/// Function: SetLivesString
/// Params:
///
/////////////////////////////////////////////////////
void SetLivesString( UIFileLoader& uiFile, int elementId, int count )
{
	const int BUFFSER_SIZE = 4;

	char finalString[BUFFSER_SIZE];

	if( count > 99 )
		count = 99;

	if( count >= 100 )
	{
		snprintf( finalString, BUFFSER_SIZE, "%d", count );
	}
	else if( count < 100 && count >= 10 )
	{
		snprintf( finalString, BUFFSER_SIZE, "%d", count );
	}
	else
	{
		snprintf( finalString, BUFFSER_SIZE, "0%d", count );
	}

	uiFile.ChangeElementText( elementId, "%s", finalString );
}

/////////////////////////////////////////////////////
/// Function: SetLevelString
/// Params:
///
/////////////////////////////////////////////////////
void SetLevelString( UIFileLoader& uiFile, int elementId, int major, int minor )
{
	const int BUFFSER_SIZE = 6;

	char finalString[BUFFSER_SIZE];
	snprintf( finalString, BUFFSER_SIZE, "%d-%d", major, minor );

	uiFile.ChangeElementText( elementId, "%s", finalString );
}

/////////////////////////////////////////////////////
/// Function: DrawFuelGauge
/// Params:
///
/////////////////////////////////////////////////////
void DrawFuelGauge( const math::Vec3& pos, const math::Vec2& dims, float fuelLevel, float low, float medium, bool isRefueling )
{
	vaPoints[0].Z = 0.0f;
	vaPoints[1].Z = 0.0f;
	vaPoints[2].Z = 0.0f;
	vaPoints[3].Z = 0.0f;

	renderer::OpenGL::GetInstance()->DisableVBO();
	renderer::OpenGL::GetInstance()->DisableTexturing();

	renderer::OpenGL::GetInstance()->EnableColourArray();
	if( isRefueling )
	{
		vaColours[3] = math::Vec4Lite( 116, 215, 223, 255 );
		vaColours[2] = math::Vec4Lite( 116, 215, 223, 255 );
		vaColours[1] = math::Vec4Lite( 48, 143, 165, 255 );
		vaColours[0] = math::Vec4Lite( 48, 143, 165, 255 );
	}
	else
	{
		if( fuelLevel >= medium )
		{
			vaColours[3] = math::Vec4Lite( 176, 201, 104, 255 );
			vaColours[2] = math::Vec4Lite( 176, 201, 104, 255 );
			vaColours[1] = math::Vec4Lite( 72, 106, 43, 255 );
			vaColours[0] = math::Vec4Lite( 72, 106, 43, 255 );
		}
		else if( fuelLevel >= low && fuelLevel < medium )
		{
			vaColours[3] = math::Vec4Lite( 255, 218, 2, 255 );
			vaColours[2] = math::Vec4Lite( 255, 218, 2, 255 );
			vaColours[1] = math::Vec4Lite( 253, 153, 0, 255 );
			vaColours[0] = math::Vec4Lite( 253, 153, 0, 255 );
		}
		else if( fuelLevel >= 0.0f && fuelLevel < low )
		{
			vaColours[3] = math::Vec4Lite( 221, 110, 0, 255 );
			vaColours[2] = math::Vec4Lite( 221, 110, 0, 255 );
			vaColours[1] = math::Vec4Lite( 161, 45, 0, 255 );
			vaColours[0] = math::Vec4Lite( 161, 45, 0, 255 );
		}
	}

	glPushMatrix();
		glTranslatef( pos.X, pos.Y, pos.Z );

		vaPoints[0] = math::Vec3( (0.0f), -(dims.Height*0.5f), 0.0f );
		vaPoints[1] = math::Vec3( (fuelLevel), -(dims.Height*0.5f), 0.0f);
		vaPoints[2] = math::Vec3( (0.0f), (dims.Height*0.5f), 0.0f);
		vaPoints[3] = math::Vec3( (fuelLevel), (dims.Height*0.5f), 0.0f );

		glVertexPointer( 3, GL_FLOAT, 0, &vaPoints[0] );
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, &vaColours[0] );

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 );
	glPopMatrix();

	renderer::OpenGL::GetInstance()->DisableColourArray();
	renderer::OpenGL::GetInstance()->EnableTexturing();

}
