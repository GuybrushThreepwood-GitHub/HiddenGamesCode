
/*===================================================================
	File: ShaderShared.cpp
	Library: Render

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"

#include "Debug/Assertion.h"
#include "Debug/DebugLogging.h"
#include "Math/Vectors.h"

#include <iostream>

#include "Render/RenderConsts.h"
#include "Render/OpenGLCommon.h"

#include "Render/GLSL/ShaderShared.h"

#ifdef BASE_SUPPORT_OPENGL_GLSL

namespace
{
	struct lstr
	{
		bool operator() (std::string s1, std::string s2) const
		{
			return( (s1.compare(s2)) < 0);
		}
	};

	struct ShaderStore
	{
		int refCount;

		GLuint vertShaderId;
		bool isVertexShaderAFile;
		char szVertexShaderFileOrString[MAX_SHADER_SIZE];

		GLuint fragShaderId;
		bool isFragmentShaderAFile;
		char szFragmentShaderFileOrString[MAX_SHADER_SIZE];

		GLuint programId;
	};

	typedef std::map< std::string, ShaderStore, struct lstr> TNameIntMap;
	typedef std::pair< std::string, ShaderStore> TNameIntPair;

	TNameIntMap ShaderMap;

	GLchar shaderSrcBuffer[MAX_SHADER_SIZE];
	GLchar tmpBuffer[MAX_SHADER_SIZE];
	char shaderLogBuffer[MAX_SHADER_LOG_SIZE];
}

/////////////////////////////////////////////////////
/// Function: ReloadShaderMap
/// Params: None
///
/////////////////////////////////////////////////////
void renderer::ReloadShaderMap()
{

}

/////////////////////////////////////////////////////
/// Function: ClearShaderMap
/// Params: None
///
/////////////////////////////////////////////////////
void renderer::ClearShaderMap()
{	
	TNameIntMap::iterator it;

	it = ShaderMap.begin();
	
	while( it != ShaderMap.end() )
	{
#ifdef _DEBUG
		DBGLOG( "*WARNING* Shader %s still active\n", it->first.c_str() );
#endif // _DEBUG

		if( glIsShader(it->second.vertShaderId) )
		{
			glDetachShader(it->second.programId, it->second.vertShaderId);
			glDeleteShader(it->second.vertShaderId);
		}

		if( glIsShader(it->second.fragShaderId) )
		{
			glDetachShader(it->second.programId, it->second.fragShaderId);
			glDeleteShader(it->second.fragShaderId);
		}

		if( glIsProgram(it->second.programId) )
			glDeleteProgram( it->second.programId );

		it++;
	}

	ShaderMap.clear();
}

/////////////////////////////////////////////////////
/// Function: RemoveShaderProgram
/// Params: [in]programId
///
/////////////////////////////////////////////////////
void renderer::RemoveShaderProgram( GLuint programId )
{
	TNameIntMap::iterator it;

	it = ShaderMap.begin();
	
	while( it != ShaderMap.end() )
	{
		if( it->second.programId == programId )
		{
			it->second.refCount--;

			if( it->second.refCount < 1 )
			{
				if( glIsShader(it->second.vertShaderId) )
				{
					glDetachShader(it->second.programId, it->second.vertShaderId);
					glDeleteShader(it->second.vertShaderId);
				}

				if( glIsShader(it->second.fragShaderId) )
				{
					glDetachShader(it->second.programId, it->second.fragShaderId);
					glDeleteShader(it->second.fragShaderId);
				}

				if( glIsProgram(it->second.programId) )
					glDeleteProgram( it->second.programId );

				ShaderMap.erase( it );

				return;
			}
		}

		it++;
	}

	ShaderMap.clear();
}

/////////////////////////////////////////////////////
/// Function: PrintInfoLog
/// Params: [in]objectId
///
/////////////////////////////////////////////////////
void renderer::PrintInfoLog( GLuint objectId )
{
	GLint infologLength = 0;
	int charsWritten  = 0;

	if( glIsShader(objectId) )
		glGetShaderiv( objectId, GL_INFO_LOG_LENGTH, &infologLength );
	else
		glGetProgramiv( objectId, GL_INFO_LOG_LENGTH, &infologLength );

	GL_CHECK 

	if( infologLength > 0 )
	{
		if( glIsShader(objectId) )
		{
			glGetShaderInfoLog( objectId, infologLength, (GLsizei *)&charsWritten, &shaderLogBuffer[0] );
			if( !core::IsEmptyString(shaderLogBuffer) )
			{
				DBGLOG("Shader InfoLog:\n%s\n", shaderLogBuffer);
				DBGLOG("************************\n");
			}
		}
		else
		{
			glGetProgramInfoLog( objectId, infologLength, (GLsizei *)&charsWritten, &shaderLogBuffer[0] );
			if( !core::IsEmptyString(shaderLogBuffer) )
			{
				DBGLOG( "Program InfoLog:\n%s\n", shaderLogBuffer );
				DBGLOG("************************\n");
			}
		}
	}
	GL_CHECK
}

/////////////////////////////////////////////////////
/// Function: LoadShader
/// Params: [in]shaderType, [in]filename
///
/////////////////////////////////////////////////////
GLuint renderer::LoadShader( GLenum shaderType, const char* szFilename )
{
	GLuint shaderId = renderer::INVALID_OBJECT;

	file::TFileHandle fileHandle;
	std::size_t fileSize = 0;

	std::memset( shaderSrcBuffer, 0, sizeof(GLchar)*MAX_SHADER_SIZE );

	// check file is valid
	if( core::IsEmptyString(szFilename) )
		return(renderer::INVALID_OBJECT);

	// valid shader type
	if( shaderType != GL_VERTEX_SHADER &&
		shaderType != GL_FRAGMENT_SHADER /*&&
		shaderType != GL_GEOMETRY_SHADER*/ )
	{
		DBGLOG( "SHADERS: *ERROR* invalid shader type\n" );
		return(renderer::INVALID_OBJECT);
	}

	// open the shader for reading
	if( !file::FileOpen( szFilename, file::FILETYPE_TEXT_READ, &fileHandle ) )
	{
		DBGLOG( "SHADERS: *ERROR* Failed loading shader: %s\n", szFilename );
		return(renderer::INVALID_OBJECT);
	}

	fileSize = file::FileSize( &fileHandle );

	// append string here
#if defined(BASE_PLATFORM_WINDOWS) || defined (BASE_PLATFORM_MAC) // normal GL
	sprintf( shaderSrcBuffer, "#version 120\n" );
#else // GL ES
	sprintf( shaderSrcBuffer, "precision highp float;\n" );
#endif

	std::size_t currentOffset = std::strlen( shaderSrcBuffer );

	// Get the shader from a file
	file::FileSeek( 0, file::FILESEEK_BEG, &fileHandle );
	std::size_t readSize = file::FileRead( &shaderSrcBuffer[currentOffset], 1, fileSize, &fileHandle );
	shaderSrcBuffer[currentOffset+readSize] = '\0';

	// close file
	file::FileClose(&fileHandle);

	// create
	shaderId = glCreateShader( shaderType );
	GLint len = static_cast<GLint>( std::strlen(shaderSrcBuffer) );
	GLchar* ptr = &shaderSrcBuffer[0];
	glShaderSource( shaderId, 1, (const GLchar**)&ptr, &len );

	GL_CHECK

	// compile it
	GLint compiled = false;
	glCompileShader( shaderId );

	GL_CHECK

	glGetShaderiv( shaderId, GL_COMPILE_STATUS, (GLint *)&compiled );

#ifdef _DEBUG
	PrintInfoLog( shaderId );
#endif // _DEBUG

	if( !compiled )
	{
		return(renderer::INVALID_OBJECT);
	}

	return(shaderId);
}

/////////////////////////////////////////////////////
/// Function: LoadShaderFromString
/// Params: [in]shaderType, [in]szShaderString
///
/////////////////////////////////////////////////////
GLuint renderer::LoadShaderFromString( GLenum shaderType, const char* szShaderString )
{
	GLuint shaderId = renderer::INVALID_OBJECT;
	std::memset( shaderSrcBuffer, 0, sizeof(GLchar)*MAX_SHADER_SIZE );

	// check file is valid
	if( core::IsEmptyString(szShaderString) )
		return(renderer::INVALID_OBJECT);

	// valid shader type
	if( shaderType != GL_VERTEX_SHADER &&
		shaderType != GL_FRAGMENT_SHADER /*&&
		shaderType != GL_GEOMETRY_SHADER*/ )
	{
		DBGLOG( "SHADERS: *ERROR* invalid shader type\n" );
		return(renderer::INVALID_OBJECT);
	}

	// append string here
#if defined(BASE_PLATFORM_WINDOWS) || defined (BASE_PLATFORM_MAC) // normal GL
	sprintf( shaderSrcBuffer, "#version 120\n" );
#else // GL ES
	sprintf( shaderSrcBuffer, "precision highp float;\n" );
#endif

	std::size_t currentOffset = std::strlen( shaderSrcBuffer );

	std::size_t strLen = std::strlen(szShaderString);
	currentOffset = snprintf( &shaderSrcBuffer[currentOffset], strLen, "%s", szShaderString );

	shaderSrcBuffer[strLen+currentOffset] = '\0';

	// create
	shaderId = glCreateShader( shaderType );
	GLint len = static_cast<GLint>( std::strlen(shaderSrcBuffer) );
	GLchar* ptr = &shaderSrcBuffer[0];
	glShaderSource( shaderId, 1, (const GLchar**)&ptr, &len );

	GL_CHECK

	// compile it
	GLint compiled = false;
	glCompileShader( shaderId );

	GL_CHECK

	glGetShaderiv( shaderId, GL_COMPILE_STATUS, (GLint *)&compiled );

#ifdef _DEBUG
	PrintInfoLog( shaderId );
#endif // _DEBUG

	if( !compiled )
	{
		return(renderer::INVALID_OBJECT);
	}

	return(shaderId);
}

/////////////////////////////////////////////////////
/// Function: LoadShaderFilesForProgram
/// Params: [in]vertexShader, [in]fragmentShader
///
/////////////////////////////////////////////////////
GLuint renderer::LoadShaderFilesForProgram( const char* vertexShader, const char* fragmentShader )
{
	if( vertexShader == 0 &&
		fragmentShader == 0 )
		return renderer::INVALID_OBJECT;

	GLuint vsId = renderer::INVALID_OBJECT;
	GLuint fsId = renderer::INVALID_OBJECT;

	std::string name;

	if( vertexShader != 0 )
		name += std::string(vertexShader);

	if( fragmentShader != 0 )
		name += std::string(fragmentShader);

	TNameIntMap::iterator it;
	it = ShaderMap.find( name );

	// did iterator not hit the end
	if( it != ShaderMap.end() )
	{
		it->second.refCount++;
		return( static_cast<GLuint>(it->second.programId) );
	}

	if( vertexShader != 0 )
		vsId = renderer::LoadShader( GL_VERTEX_SHADER, vertexShader );

	if( fragmentShader != 0 )
		fsId = renderer::LoadShader( GL_FRAGMENT_SHADER, fragmentShader );

	if( vsId != renderer::INVALID_OBJECT ||
		fsId != renderer::INVALID_OBJECT )
	{
		// create and attach
		GLuint progId = glCreateProgram();

		if( vsId != renderer::INVALID_OBJECT )
			glAttachShader( progId, vsId );
	
		if( fsId != renderer::INVALID_OBJECT )
			glAttachShader( progId, fsId );

		// Link the program object 
		glLinkProgram( progId );
			
		GL_CHECK

		// was linked
		GLuint linked = false;
		glGetProgramiv( progId, GL_LINK_STATUS, (GLint *)&linked );

#ifdef _DEBUG
        GLint status = GL_TRUE;
        glValidateProgram(progId);
        
        glGetProgramiv(progId, GL_VALIDATE_STATUS, &status);
        if (status == GL_FALSE)
        {
            renderer::PrintInfoLog( progId );
            DBGLOG( "SHADERS: status %d\n", status );
            return(renderer::INVALID_OBJECT);
        }
		
		DBGLOG( "v: %s | f:%s\n", vertexShader, fragmentShader );
		renderer::PrintInfoLog( progId );
#endif // _DEBUG

		if( !linked )
		{
			return(renderer::INVALID_OBJECT);
		}

		GL_CHECK

		ShaderStore addProgram;
		addProgram.refCount = 1;
		addProgram.programId = progId;
		addProgram.vertShaderId = vsId;
		addProgram.fragShaderId = fsId;
		addProgram.isFragmentShaderAFile = true;
		addProgram.isVertexShaderAFile = true;
		snprintf( addProgram.szVertexShaderFileOrString, MAX_SHADER_SIZE, "%s", vertexShader );
		snprintf( addProgram.szFragmentShaderFileOrString, MAX_SHADER_SIZE, "%s", fragmentShader );

		ShaderMap.insert( TNameIntPair( name, addProgram ) );

		return progId;
	}
	
	return renderer::INVALID_OBJECT;
}

/////////////////////////////////////////////////////
/// Function: LoadShaderStringsForProgram
/// Params: [in]vertexShaderString, [in]fragmentShaderString
///
/////////////////////////////////////////////////////
GLuint renderer::LoadShaderStringsForProgram(const char* vertexShaderString, const char* fragmentShaderString )
{
	if( core::IsEmptyString( vertexShaderString ) &&
		core::IsEmptyString( fragmentShaderString ) )
	{
		return renderer::INVALID_OBJECT;
	}

	std::string name;
	std::size_t copyCount = 32;

	if( !core::IsEmptyString( vertexShaderString ) )
	{
		if( std::strlen(vertexShaderString) < 32 )
			copyCount = std::strlen(vertexShaderString);

		snprintf( shaderSrcBuffer, copyCount, "%s", vertexShaderString );
	}

	if( !core::IsEmptyString( fragmentShaderString ) )
	{
		if( std::strlen(fragmentShaderString) < 32 )
			copyCount = std::strlen(fragmentShaderString);

		snprintf( tmpBuffer, copyCount, "%s", fragmentShaderString );
	}

	name += std::string(shaderSrcBuffer);
	name += std::string(tmpBuffer);

	TNameIntMap::iterator it;
	it = ShaderMap.find( name );

	// did iterator not hit the end
	if( it != ShaderMap.end() )
	{
		it->second.refCount++;
		return( static_cast<GLuint>(it->second.programId) );
	}

	// first load the vertex shader in
	GLuint vsId = renderer::INVALID_OBJECT;
	GLuint fsId = renderer::INVALID_OBJECT;

	if( !core::IsEmptyString( vertexShaderString ) )
		vsId = renderer::LoadShaderFromString( GL_VERTEX_SHADER, vertexShaderString );

	if( !core::IsEmptyString( fragmentShaderString ) )
		fsId = renderer::LoadShaderFromString( GL_FRAGMENT_SHADER, fragmentShaderString );

	if( vsId != renderer::INVALID_OBJECT ||
		fsId != renderer::INVALID_OBJECT )
	{
		// create and attach
		GLuint progId = glCreateProgram();

		if( vsId != renderer::INVALID_OBJECT )
			glAttachShader( progId, vsId );

		if( fsId != renderer::INVALID_OBJECT )
			glAttachShader( progId, fsId );

		// Link the program object 
		glLinkProgram( progId );
			
		GL_CHECK

		// was linked
		GLuint linked = false;
		glGetProgramiv( progId, GL_LINK_STATUS, (GLint *)&linked );

#ifdef _DEBUG
		GLint status = GL_TRUE;
		glValidateProgram(progId);
		
		glGetProgramiv(progId, GL_VALIDATE_STATUS, &status);
		if (status == GL_FALSE)
		{
            renderer::PrintInfoLog( progId );
			DBGLOG( "SHADERS: status %d\n", status );
			return(renderer::INVALID_OBJECT);
		}
		
		renderer::PrintInfoLog( progId );
#endif // _DEBUG

		if( !linked )
		{
			return(renderer::INVALID_OBJECT);
		}

		GL_CHECK

		ShaderStore addProgram;
		addProgram.refCount = 1;
		addProgram.programId = progId;
		addProgram.vertShaderId = vsId;
		addProgram.fragShaderId = fsId;
		addProgram.isFragmentShaderAFile = false;
		addProgram.isVertexShaderAFile = false;
		snprintf( addProgram.szVertexShaderFileOrString, MAX_SHADER_SIZE, "%s", vertexShaderString );
		snprintf( addProgram.szFragmentShaderFileOrString, MAX_SHADER_SIZE, "%s", fragmentShaderString );

		ShaderMap.insert( TNameIntPair( name, addProgram ) );

		return progId;
	}
	
	return renderer::INVALID_OBJECT;
}

#endif // BASE_SUPPORT_OPENGL_GLSL
