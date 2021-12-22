
#include "ShaderProgram.h"
#include "FileUtils.h"
#include "UniformBuffer.h"

#include <fstream>
#include <sstream>
#include <iostream>

static bool HasErrors(const unsigned int& Id, const FString& ShaderType, const char* Filename)
{
	int success;
	char infoLog[1024];
	if (ShaderType != "PROGRAM")
	{
		glGetShaderiv(Id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(Id, 1024, NULL, infoLog);
			std::cout << "Shader compilation failed. [" << Filename << "]"
					  << "\n" << infoLog<< std::endl;
			return true;
		}
	}
	else
	{
		glGetProgramiv(Id, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(Id, 1024, NULL, infoLog);
			std::cout << "Shader compilation failed. [PROGRAM_LINK_FAIL] [" << Filename << "]"
					<< "\n" << infoLog << std::endl;
			return true;
		}
	}

	return false;
}

static FString ReadShaderCode(const char* FilePath)
{
	FString shaderCode;
	
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	
	try
	{
		shaderFile.open(FilePath);
		
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		
		shaderFile.close();
		
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure&)
	{
		std::cout << "Shader compilation failed. Reason [FILE_READ] [" << FilePath << "]" << std::endl;
	}
	
	return shaderCode;
}

bool CompileShader(uint32& outId, const char* ShaderCode, GLuint ShaderType, const char* ShaderFriendlyName, const char* FilePath)
{
	uint32 id = glCreateShader(ShaderType);
	glShaderSource(id, 1, &ShaderCode, NULL);
	glCompileShader(id);

	if(HasErrors(id, ShaderFriendlyName, FilePath))
	{
		glDeleteShader(id);
		return false;
	}
	else
	{
		outId = id;
		return true;
	}
}

FShaderProgram::FShaderProgram(const char* VertexFilePath, const char* GeometryFilePath, const char* FragmentFilePath)
	: Id(0)
	, bIsEnabled(false)
	, bIsInitialized(false)
{
	// 2. compile shaders
	uint32 vertexId, geometryId, fragmentId;
	
	// vertex shader
	if(!CompileShader(
		vertexId,
		ReadShaderCode(VertexFilePath).c_str(), 
		GL_VERTEX_SHADER, 
		"VERTEX", 
		VertexFilePath))
	{
		return;
	}
	
	if(GeometryFilePath && !CompileShader(
		geometryId,
		ReadShaderCode(GeometryFilePath).c_str(), 
		GL_GEOMETRY_SHADER,
		"GEOMETRY", 
		GeometryFilePath))
	{
		glDeleteShader(vertexId);
		return;
	}
	
	if(!CompileShader(
		fragmentId,
		ReadShaderCode(FragmentFilePath).c_str(), 
		GL_FRAGMENT_SHADER,
		"FRAGMENT", 
		FragmentFilePath))
	{
		glDeleteShader(vertexId);
		if(GeometryFilePath) glDeleteShader(geometryId);
		return;
	}

	// shader Program
	Id = glCreateProgram();
	glAttachShader(Id, vertexId);
	if(GeometryFilePath) glAttachShader(Id, geometryId);
	glAttachShader(Id, fragmentId);
	glLinkProgram(Id);

	if(HasErrors(Id, "PROGRAM", FragmentFilePath))
	{
		glDeleteShader(vertexId);
		if(GeometryFilePath) glDeleteShader(geometryId);
		glDeleteShader(fragmentId);
		glDeleteProgram(Id);
		return;
	}
	
	glDeleteShader(vertexId);
	if(GeometryFilePath) glDeleteShader(geometryId);
	glDeleteShader(fragmentId);
	
	std::cout << "Shader compiled successfully" << std::endl;
	std::cout << "Vertex: " << VertexFilePath << std::endl;
	if(GeometryFilePath) { std::cout << "Geometry: " << GeometryFilePath << std::endl; }
	else { std::cout << "Geometry: " << "<not_used>" << std::endl; }
	std::cout << "Fragment: " << FragmentFilePath << std::endl;
	std::cout << std::endl;
	
	bIsInitialized = true;
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

void FShaderProgram::SetBool(const char* Name, bool Value)
{
	glUniform1i(glGetUniformLocation(Id, Name), (int)Value);
}

void FShaderProgram::SetInt32(const char* Name, const int32& Value)
{
	glUniform1i(glGetUniformLocation(Id, Name), Value);
}

void FShaderProgram::SetUInt32(const char* Name, const uint32& Value)
{
	glUniform1ui(glGetUniformLocation(Id, Name), Value);
}

void FShaderProgram::SetFloat(const char* Name, float Value)
{
	glUniform1f(glGetUniformLocation(Id, Name), Value);
}

void FShaderProgram::SetVec3(const char *Name, const glm::vec3& Value)
{
	glUniform3fv(glGetUniformLocation(Id, Name), 1, &Value[0]);
}

void FShaderProgram::SetVec4(const char *Name, const glm::vec4& Value)
{
	glUniform4fv(glGetUniformLocation(Id, Name), 1, &Value[0]);
}

void FShaderProgram::SetMat3(const char *Name, const glm::mat3& Value)
{
	glUniformMatrix3fv(glGetUniformLocation(Id, Name), 1, GL_FALSE, &Value[0][0]);
}

void FShaderProgram::SetMat4(const char* Name, const glm::mat4& Value)
{
	glUniformMatrix4fv(glGetUniformLocation(Id, Name), 1, GL_FALSE, &Value[0][0]);
}

void FShaderProgram::SetSampler2D(const char* Name, const FTextureId Value)
{
	glUniform1i(glGetUniformLocation(Id, Name), Value);
}

bool FShaderProgram::SetUniformBuffer(const char* Name, const FUniformBuffer& Value)
{
	uint32 idx = glGetUniformBlockIndex(Id, Name);
	if(idx == GL_INVALID_INDEX) return false;
	else if(idx == Value.GetPointIdx()) return true;
	
	glUniformBlockBinding(Id, idx, Value.GetPointIdx());
	
	return true;
}