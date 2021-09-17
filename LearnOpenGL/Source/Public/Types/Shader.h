
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

public: // Constructors

    FShaderProgram(const char* VertexFilePath, const char* FragmentFilePath);
    virtual ~FShaderProgram();

public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; }
	inline bool IsUsed() const { return bIsUsed; }
    inline const FShaderProgramId& GetProgramId() const { return Id; }

public: // External methods

	void Use();
	void Clear();

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

	bool bIsUsed;
	bool bIsInitialized;
};