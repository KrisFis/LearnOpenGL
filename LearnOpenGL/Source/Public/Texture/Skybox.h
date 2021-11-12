
#pragma once

#include "ModuleMinimal.h"
#include "SceneObject.h"

// Forward declarations
class FCubemap;

class FSkybox : public ISceneObject
{
	
private: // Constructors

	explicit FSkybox(const TSharedPtr<FCubemap>& InMap);

public: // Destructor

	virtual ~FSkybox();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FSkybox> Create(const TSharedPtr<FCubemap>& Map)
	{ return MakeShareable(new FSkybox(Map)); }

public: // Getters

	FORCEINLINE bool IsInitialized() const { return bIsInitialized; }
	FORCEINLINE const TSharedPtr<FCubemap>& GetMap() const { return Map; }

public: // Setters

	void SetMap(const TSharedPtr<FCubemap>& InMap);

public: // ISceneObject overrides

	virtual bool CullsFaces() const { return false; }
	virtual void SetCullFaces(bool Value) { ENSURE_NO_ENTRY(); /* Unsupported */ }
	virtual bool IsOutlined() const { return false; }
	virtual void SetOutlineColor(const FColor& Value) { ENSURE_NO_ENTRY(); /* Unsupported */ }
	virtual void SetOutlineSize(float Value) { ENSURE_NO_ENTRY(); /* Unsupported */ }
	virtual FTransform GetTransform() const { return FTransform(); }
	virtual void SetTransform(const FTransform& Value) { ENSURE_NO_ENTRY(); /* Unsupported */ }
	
	virtual void Draw(const TSharedPtr<FShaderProgram>& Shader);

private: // Fields

	TSharedPtr<FCubemap> Map;

	FVertexArrayId VAO;
	FBufferId VBO;

private: // Primitive fields

	uint8 bIsInitialized : 1;
};

typedef TSharedPtr<FSkybox> FSkyboxPtr;