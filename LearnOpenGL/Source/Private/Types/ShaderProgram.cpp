
#include "ShaderProgram.h"
#include "FileUtils.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <assert.h>

static bool HasErrors(const unsigned int& Id, const FString& ShaderType)
{
    int success;
    char infoLog[1024];
    if (ShaderType != "PROGRAM")
    {
        glGetShaderiv(Id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(Id, 1024, NULL, infoLog);
            std::cout << "Shader compilation failed. [" << ShaderType << "_SHADER_COMPILE_FAIL]\nLog: " << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            return true;
        }
    }
    else
    {
        glGetProgramiv(Id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(Id, 1024, NULL, infoLog);
            std::cout << "Shader compilation failed. [PROGRAM_LINK_FAIL]\nLog: " << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            return true;
        }
    }

    return false;
}

FShaderProgram::FShaderProgram(const char* VertexFilePath, const char* FragmentFilePath)
	: Id(0)
	, bIsEnabled(false)
	, bIsInitialized(false)
{
	// 1. retrieve the vertex/fragment source code from filePath
	FString vertexCode;
	FString fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

	// Prepand with shaders folder
	//FString fullVertexPath = NFileUtils::ContentPath() + FString("/Shaders/Vertex/") + FString(VertexFilePath);
	//FString fullFragmentPath = NFileUtils::ContentPath() + FString("/Shaders/Fragment/") + FString(FragmentFilePath);

	try
	{
		 // open files
		 vShaderFile.open(VertexFilePath);
		 fShaderFile.open(FragmentFilePath);
		 std::stringstream vShaderStream, fShaderStream;
		 // read file's buffer contents into streams
		 vShaderStream << vShaderFile.rdbuf();
		 fShaderStream << fShaderFile.rdbuf();
		 // close file handlers
		 vShaderFile.close();
		 fShaderFile.close();
		 // convert stream into string
		 vertexCode   = vShaderStream.str();
		 fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure&)
	{
		std::cout << "Shader compilation failed. Reason [FILE_READ]" << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// 2. compile shaders
	unsigned int vertexId, fragmentId;

	bool hasErrors = false;

	// vertex shader
	vertexId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexId, 1, &vShaderCode, NULL);
	glCompileShader(vertexId);

	hasErrors = hasErrors || HasErrors(vertexId, "VERTEX");

	// fragment Shader
	fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentId, 1, &fShaderCode, NULL);
	glCompileShader(fragmentId);

	hasErrors = hasErrors || HasErrors(fragmentId, "FRAGMENT");

	// shader Program
	Id = glCreateProgram();
	glAttachShader(Id, vertexId);
	glAttachShader(Id, fragmentId);
	glLinkProgram(Id);

	hasErrors = hasErrors || HasErrors(Id, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertexId);
	glDeleteShader(fragmentId);

	if(!hasErrors)
	{
		std::cout << "Shader compiled successfully [vertex: " << VertexFilePath << ", fragment: " << FragmentFilePath << "]." << std::endl;
		bIsInitialized = true;
	}
}

FShaderProgram::~FShaderProgram()
{
	if(Id != NRenderConsts::Invalid::ShaderProgramId)
		glDeleteProgram(Id);
}

void FShaderProgram::Enable()
{
	glUseProgram(Id);

	bIsEnabled = true;
}

void FShaderProgram::Disable()
{
	glUseProgram(0);

	bIsEnabled = false;
}

void FShaderProgram::SetBool(const char* Name, bool Value) const
{
	glUniform1i(glGetUniformLocation(Id, Name), (int)Value);
}

void FShaderProgram::SetInt32(const char* Name, const int32& Value) const
{
	glUniform1i(glGetUniformLocation(Id, Name), Value);
}

void FShaderProgram::SetUInt32(const char* Name, const uint32& Value) const
{
	glUniform1ui(glGetUniformLocation(Id, Name), Value);
}

void FShaderProgram::SetFloat(const char* Name, float Value) const
{
	glUniform1f(glGetUniformLocation(Id, Name), Value);
}

void FShaderProgram::SetVec3(const char *Name, const glm::vec3& Value) const
{
	glUniform3fv(glGetUniformLocation(Id, Name), 1, &Value[0]);
}

void FShaderProgram::SetVec4(const char *Name, const glm::vec4& Value) const
{
	glUniform4fv(glGetUniformLocation(Id, Name), 1, &Value[0]);
}

void FShaderProgram::SetMat3(const char *Name, const glm::mat3& Value) const
{
	glUniformMatrix3fv(glGetUniformLocation(Id, Name), 1, GL_FALSE, &Value[0][0]);
}

void FShaderProgram::SetMat4(const char* Name, const glm::mat4& Value) const
{
	glUniformMatrix4fv(glGetUniformLocation(Id, Name), 1, GL_FALSE, &Value[0][0]);
}