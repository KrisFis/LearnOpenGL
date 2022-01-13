
#pragma once

#include "ModuleMinimal.h"

// Forward declaration
class FUniformBuffer;

enum class EShaderType
{
	Invalid = 0,
	Vertex,
	Fragment
};

class FShaderProgram
{

private: // Constructors

    explicit FShaderProgram(const char* VertexFilePath, const char* GeometryFilePath, const char* FragmentFilePath);

public: // Destructors
    
    virtual ~FShaderProgram();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FShaderProgram> Create(const char* VertexFilePath, const char* FragmentFilePath)
	{ return MakeShareable(new FShaderProgram(VertexFilePath, nullptr, FragmentFilePath)); }
	
	FORCEINLINE static TSharedPtr<FShaderProgram> Create(const char* VertexFilePath, const char* GeometryFilePath, const char* FragmentFilePath)
	{ return MakeShareable(new FShaderProgram(VertexFilePath, GeometryFilePath, FragmentFilePath)); }

public: // Getters

	FORCEINLINE bool IsInitialized() const { return bIsInitialized; }
	FORCEINLINE bool IsEnabled() const { return bIsEnabled; }
	FORCEINLINE const FShaderProgramId& GetProgramId() const { return Id; }

public: // External methods

	void Enable();
	void Disable();

	void SetBool(const char* Name, bool Value);
	void SetInt32(const char* Name, const int32& Value);
	void SetUInt32(const char* Name, const uint32& Value);
	void SetFloat(const char* Name, float Value);
	void SetVec2(const char* Name, const glm::vec2& Value);
	void SetVec3(const char* Name, const glm::vec3& Value);
	void SetVec4(const char* Name, const glm::vec4& Value);
	void SetMat3(const char* Name, const glm::mat3& Value);
	void SetMat4(const char* Name, const glm::mat4& Value);
	
	bool SetUniformBuffer(const char* Name, const FUniformBuffer& Value);

private: // Fields

	FShaderProgramId Id;

	bool bIsEnabled;
	bool bIsInitialized;
};

typedef TSharedPtr<FShaderProgram> FShaderProgramPtr;