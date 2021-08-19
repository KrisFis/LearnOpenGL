
#pragma once

#include <GLAD/glad.h>
#include <glm/glm.hpp>

class FShader
{

public: // Constructors

    FShader(const char* VertexFilename, const char* FragmentFilename);
    virtual ~FShader();

public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; }
	inline bool IsUsed() const { return bIsUsed; }
    inline const unsigned int& GetProgramId() const { return ProgramId; }

public: // External methods

	void Use();
	void Clear();

	void SetBool(const char* Name, bool Value) const;
	void SetInt(const char* Name, const int& Value) const;
	void SetUInt(const char* Name, const unsigned int& Value) const;
	void SetFloat(const char* Name, float Value) const;
	void SetMatrix4(const char* Name, const glm::mat4& Value) const;

private: // Fields

    unsigned int ProgramId;

	bool bIsUsed;
	bool bIsInitialized;
};