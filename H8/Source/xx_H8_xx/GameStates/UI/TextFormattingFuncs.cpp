
/*===================================================================
	File: TextFormattingFuncs.cpp
	Game: H8

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

#include "H8Consts.h"
#include "H8.h"

#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/UI/TextFormattingFuncs.h"

namespace
{
	math::Vec3 vaPoints[4];
	math::Vec4Lite vaColours[4];
	char htmlText[UI_MAXSTATICTEXTBUFFER_SIZE];
}

/////////////////////////////////////////////////////
/// Function: SetTime
/// Params:
///
/////////////////////////////////////////////////////
void SetTime( UIFileLoader& uiFile, int elementId, float timer )
{
	char text[UI_MAXSTATICTEXTBUFFER_SIZE];

	int minAsInt = static_cast<int>(timer / 60.0f);
	int secsAsInt = static_cast<int>(timer) % 60;
	int milliSecsAsInt = static_cast<int>( (timer-(static_cast<float>(minAsInt)*60.0f)-static_cast<float>(secsAsInt)) * 100.0f );

	if( minAsInt > 99 )
		minAsInt = 99;

	if( secsAsInt < 10 )
	{
		if( milliSecsAsInt < 10 )
		{
			if( minAsInt < 10 )
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "0%d:0%d.0%d", minAsInt, secsAsInt, milliSecsAsInt );
			else
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:0%d.0%d", minAsInt, secsAsInt, milliSecsAsInt );
		}
		else
		{
			if( minAsInt < 10 )
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "0%d:0%d.%d", minAsInt, secsAsInt, milliSecsAsInt );
			else
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:0%d.%d", minAsInt, secsAsInt, milliSecsAsInt );
		}
	}
	else
	{
		if( milliSecsAsInt < 10 )
		{
			if( minAsInt < 10 )
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "0%d:%d.0%d", minAsInt, secsAsInt, milliSecsAsInt );
			else
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:%d.0%d", minAsInt, secsAsInt, milliSecsAsInt );
		}
		else
		{
			if( minAsInt < 10 )
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "0%d:%d.%d", minAsInt, secsAsInt, milliSecsAsInt );
			else
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:%d.%d", minAsInt, secsAsInt, milliSecsAsInt );
		}
	}

	uiFile.ChangeElementText( elementId, "%s", text );

}

/////////////////////////////////////////////////////
/// Function: SetTime
/// Params:
///
/////////////////////////////////////////////////////
void SetScore( UIFileLoader& uiFile, int elementId, unsigned long long int score )
{
	int i=0, j=0, k=0;

	char tmpString[UI_MAXSTATICTEXTBUFFER_SIZE];
	char scoreToText[UI_MAXSTATICTEXTBUFFER_SIZE];
	
	std::memset( scoreToText, 0, sizeof(char)*UI_MAXSTATICTEXTBUFFER_SIZE );

	snprintf( tmpString, UI_MAXSTATICTEXTBUFFER_SIZE, "%llu", score );

	int len = static_cast<int>(std::strlen(tmpString));

	if( len > 0 )
	{
		int extraDigits = 0;

		// how many digits?
		if( len > 3 )
		{
			extraDigits = len / 3;
			if( extraDigits > 1 &&
				(len % 3 == 0) )
			{
				extraDigits -= 1;
			}
		}

		for( i=len, j=len+extraDigits, k=0; i >= 0; i-- )
		{
			if( i == 0 )
			{
				scoreToText[j] = tmpString[0];
			}
			else
			{
				scoreToText[j] = tmpString[i];
				j--;

				if( tmpString[i] != '/0' )
					k++;

				if( k==4 &&
					extraDigits > 0 )
				{
					scoreToText[j] = ',';
					k=1;
					j--;
				}
			}
		}

		uiFile.ChangeElementText( elementId, "%s", scoreToText );
	}
}

/////////////////////////////////////////////////////
/// Function: DrawDownloadBar
/// Params:
///
/////////////////////////////////////////////////////
void DrawDownloadBar( const math::Vec3& pos, const math::Vec2& dims, float currentTime, float maxTime, UIFileLoader& uiFile, int elementId  )
{
	float timeToPercent = (100.0f / maxTime) * currentTime;
	float totalWidth = (dims.Width / 100.0f) * timeToPercent;

	renderer::OpenGL::GetInstance()->DisableVBO();

	bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();
	if( textureState )
		renderer::OpenGL::GetInstance()->DisableTexturing();

	renderer::OpenGL::GetInstance()->SetColour4ub(0, 193, 3, 255);

	//renderer::OpenGL::GetInstance()->EnableColourArray();
	//vaColours[3] = math::Vec4Lite( 0, 193, 3, 255 );
	//vaColours[2] = math::Vec4Lite( 0, 193, 3, 255 );
	//vaColours[1] = math::Vec4Lite( 0, 193, 3, 255 );
	//vaColours[0] = math::Vec4Lite( 0, 193, 3, 255 );

	glPushMatrix();
		glTranslatef( pos.X, pos.Y, pos.Z );

		vaPoints[0] = math::Vec3( (0.0f), -(dims.Height*0.5f), 0.0f );
		vaPoints[1] = math::Vec3( (totalWidth), -(dims.Height*0.5f), 0.0f);
		vaPoints[2] = math::Vec3( (0.0f), (dims.Height*0.5f), 0.0f);
		vaPoints[3] = math::Vec3( (totalWidth), (dims.Height*0.5f), 0.0f );

		glVertexPointer( 3, GL_FLOAT, 0, &vaPoints[0] );
		//glColorPointer(4, GL_UNSIGNED_BYTE, 0, &vaColours[0] );

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 );
	glPopMatrix();

	//renderer::OpenGL::GetInstance()->DisableColourArray();
	if( textureState )
		renderer::OpenGL::GetInstance()->EnableTexturing();

	math::Vec3 offset;
	offset.X = vaPoints[3].X + 4;

	uiFile.ChangeElementPosition( elementId, pos+offset );
	uiFile.ChangeElementText( elementId, "%d%%%%", static_cast<int>(timeToPercent) );
}

/////////////////////////////////////////////////////
/// Function: FormatScoreToHTML
/// Params:
///
/////////////////////////////////////////////////////
const char* FormatScoreToHTML( unsigned long long int score )
{
	int i=0, j=0, k=0;

	char tmpString[UI_MAXSTATICTEXTBUFFER_SIZE];
	
	std::memset( htmlText, 0, sizeof(char)*UI_MAXSTATICTEXTBUFFER_SIZE );

	snprintf( tmpString, UI_MAXSTATICTEXTBUFFER_SIZE, "%llu", score );

	int len = static_cast<int>(std::strlen(tmpString));

	if( len > 0 )
	{
		int extraDigits = 0;

		// how many digits?
		if( len > 3 )
		{
			extraDigits = (len / 3)*3;
			if( extraDigits > (1*3) &&
				(len % 3 == 0) )
			{
				extraDigits -= (1*3);
			}
		}

		for( i=len, j=len+extraDigits, k=0; i >= 0; i-- )
		{
			if( i == 0 )
			{
				htmlText[j] = tmpString[0];
			}
			else
			{
				htmlText[j] = tmpString[i];
				j--;

				if( tmpString[i] != '/0' )
					k++;

				if( k==4 &&
					extraDigits > 0 )
				{
					htmlText[j] = 'C';
					htmlText[j-1] = '2';
					htmlText[j-2] = '%';
					k=1;
					j-=3;
				}
			}
		}

		return htmlText;
	}

	return 0;
}
