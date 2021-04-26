
#pragma once

#include <GLAD/glad.h>

class FShader
{

public: // Constructors

    FShader(const char* VertexFilename, const char* FragmentFilename);
    virtual ~FShader();

public: // Getters

	inline bool IsInitialized() const { return bIsInitialized; }
    inline const unsigned int& GetProgramId() const { return ProgramId; }

public: // External methods

	void Apply();

	void SetBool(const char* Name, bool Value) const;
	void SetInt(const char* Name, const int& Value) const;
	void SetUInt(const char* Name, const unsigned int& Value) const;
	void SetFloat(const char* Name, float Value) const;

private: // Fields

    unsigned int ProgramId;

	bool bIsInitialized;
};