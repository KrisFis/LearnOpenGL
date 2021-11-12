
#pragma once

#include "ModuleMinimal.h"

// Forward declare
class FShaderProgram;
struct FColor;

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

class ISceneObject : public TSharedClass<ISceneObject>
{

public: // Culling

	virtual bool CullsFaces() const = 0;
	virtual void SetCullFaces(bool Value) = 0;

public: // Outline

	virtual bool IsOutlined() const = 0;
	virtual void SetOutlineColor(const FColor& Value) = 0;
	virtual void SetOutlineSize(float Value) = 0;

public: // Transform

	virtual FTransform GetTransform() const = 0;
	virtual void SetTransform(const FTransform& Value) = 0;

public: // Draw methods

	virtual void Draw(const TSharedPtr<FShaderProgram>& Shader) = 0;

};

typedef TSharedPtr<class ISceneObject> FSceneObjectPtr;