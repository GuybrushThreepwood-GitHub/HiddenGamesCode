
#ifndef __SHADERSHARED_H__
#define __SHADERSHARED_H__

#ifdef BASE_SUPPORT_OPENGL_GLSL

// forward declare
namespace renderer { class Shader; }

const int MAX_SHADER_SIZE		= (16*1024);
const int MAX_SHADER_LOG_SIZE	= (32*1024);

namespace renderer
{
	enum EShaderType
	{
		SHADERTYPE_UNKNOWN=0,
		
		SHADERTYPE_VERTEX_SHADER=1,
		SHADERTYPE_FRAGMENT_SHADER=2,
		SHADERTYPE_GEOMETRY_SHADER=3,

		SHADERTYPE_MISSING_UNSUPPORTED=9999,
	};


	/// ReloadShaderMap - attempts to reload all shader programs into their designated slots
	void ReloadShaderMap();
	/// ClearTextureMap - clears the shader name map
	void ClearShaderMap();
	/// RemoveShaderProgram - removes a shader program from the map
	/// \param programId - program object id
	void RemoveShaderProgram( GLuint programId );

	/// PrintInfoLog - prints a shader or program log
	void PrintInfoLog( GLuint objectId );

	/// LoadShader - Loads a shader from a file
	/// \param filename - name of file to load
	/// \param shaderType - what type of shader
	/// \return integer - (SUCCESS: GL shader id, FAIL: INVALID_OBJECT)
	GLuint LoadShader( GLenum shaderType, const char *filename );
	/// LoadShader - Loads a shader from a string
	/// \param filename - name of file to load
	/// \param shaderType - what type of shader
	/// \return integer - (SUCCESS: GL shader id, FAIL: INVALID_OBJECT)
	GLuint LoadShaderFromString( GLenum shaderType, const char* szShaderString );
	/// LoadShaderFilesForProgram - Loads a vertex and fragment shader to create a program
	/// \param vertexShader - vertex shader file
	/// \param fragmentShader - fragment shader file
	/// \return integer - (SUCCESS: GL shader id, FAIL: INVALID_OBJECT)
	GLuint LoadShaderFilesForProgram(const char* vertexShader, const char* fragmentShader );
	/// LoadShaderStringsForProgram - Loads a vertex and fragment shader from strings to create a program
	/// \param vertexShaderString - vertex shader string
	/// \param fragmentShaderString - fragment shader string
	/// \return integer - (SUCCESS: GL shader id, FAIL: INVALID_OBJECT)
	GLuint LoadShaderStringsForProgram(const char* vertexShaderString, const char* fragmentShaderString );


} // namespace renderer

#endif // BASE_SUPPORT_OPENGL_GLSL

#endif // __SHADERSHARED_H__

