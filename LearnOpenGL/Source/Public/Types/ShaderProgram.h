
#pragma once

#include "ModuleMinimal.h"

enum class EShaderType
{
	Invalid = 0,
	Vertex,
	Fragment
};

class FShaderProgram
{

private: // Constructors

    explicit FShaderProgram(const char* VertexFilePath, const char* FragmentFilePath);

public: // Destructors
    
    virtual ~FShaderProgram();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FShaderProgram> Create(const char* VertexFilePath, const char* FragmentFilePath)
	{ return MakeShareable(new FShaderProgram(VertexFilePath, FragmentFilePath)); }

public: // Getters

	FORCEINLINE bool IsInitialized() const { return bIsInitialized; }
	FORCEINLINE bool IsEnabled() const { return bIsEnabled; }
    FORCEINLINE const FShaderProgramId& GetProgramId() const { return Id; }

public: // External methods

	void Enable();
	void Disable();

	void SetBool(const char* Name, bool Value) const;
	void SetInt32(const char* Name, const int32& Value) const;
	void SetUInt32(const char* Name, const uint32& Value) const;
	void SetFloat(const char* Name, float Value) const;
	void SetVec3(const char* Name, const glm::vec3& Value) const;
	void SetVec4(const char* Name, const glm::vec4& Value) const;
	void SetMat3(const char* Name, const glm::mat3& Value) const;
	void SetMat4(const char* Name, const glm::mat4& Value) const;

private: // Fields

    FShaderProgramId Id;

	bool bIsEnabled;
	bool bIsInitialized;
};

typedef TSharedPtr<FShaderProgram> FShaderProgramPtr;