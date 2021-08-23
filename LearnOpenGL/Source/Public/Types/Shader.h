
#pragma once

#include "ModuleMinimal.h"

class FShader
{

public: // Constructors

    FShader(const char* VertexFilename, const char* FragmentFilename);
    virtual ~FShader();

public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; }
	inline bool IsUsed() const { return bIsUsed; }
    inline const FShaderId& GetProgramId() const { return ProgramId; }

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

    FShaderId ProgramId;

	bool bIsUsed;
	bool bIsInitialized;
};