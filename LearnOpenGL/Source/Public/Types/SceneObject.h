
#pragma once

#include "ModuleMinimal.h"

class FShaderProgram;

struct FTransform
{
	FTransform() = default;
	FTransform(const glm::vec3& InPos, const glm::vec3& InRot, const glm::vec3& InScale)
		: Position(InPos)
		, Rotation(InRot)
		, Scale(InScale)
	{}
	
	glm::mat4 CalculateModelMatrix() const;

	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;
};

class ISceneObject
{

public: // Transform

	virtual FTransform GetTransform() const = 0;
	virtual void SetTransform(const FTransform& Value) = 0;

public: // Draw methods

	virtual void Draw(FShaderProgram& Shader) = 0;

};