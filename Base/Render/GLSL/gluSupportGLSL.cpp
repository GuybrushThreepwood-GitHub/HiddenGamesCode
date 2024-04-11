
/*===================================================================
 File: gluSupportGLSL.cpp
 Library: Render
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_SUPPORT_OPENGL_GLSL

#include "CoreBase.h"

#include <cstring>
#include <cmath>

#include "Math/MathConsts.h"
#include "Math/Vectors.h"

#include "Render/RenderConsts.h"
#include "Render/OpenGLCommon.h"
#include "Render/GLSL/gluSupportGLSL.h"

namespace gluUtil
{
	struct token_string
	{
	   GLuint Token;
	   const char *String;
	};

	static const struct token_string Errors[] = 
	{
	   { GL_NO_ERROR,			"GL_NO_ERROR"			}/*,
	   { GL_INVALID_ENUM,		"GL_INVALID_ENUM"		},
	   { GL_INVALID_VALUE,		"GL_INVALID_VALUE"		},
	   { GL_INVALID_OPERATION,	"GL_INVALID_OPERATION"	},
	   { GL_STACK_OVERFLOW,		"GL_STACK_OVERFLOW"		},
	   { GL_STACK_UNDERFLOW,	"GL_STACK_UNDERFLOW"	},
	   { GL_OUT_OF_MEMORY,		"GL_OUT_OF_MEMORY"		}*/
	};

} // namespace gluUtil


/////////////////////////////////////////////////////
/// Method: gluErrorString
/// Params: [in]error
///
/////////////////////////////////////////////////////
const GLubyte * gluErrorString (GLenum errorCode)
{
    int i;
	for( i = 0; gluUtil::Errors[i].String; i++ ) 
	{
        if (gluUtil::Errors[i].Token == errorCode)
            return (const GLubyte *)gluUtil::Errors[i].String;
    }

	return 0;
}

#endif // BASE_SUPPORT_OPENGL_GLSL
