
#pragma once

#include "ModuleMinimal.h"

// Forward declaration
class ISceneObject;

class FScene
{
	
private: // Constructors

	explicit FScene();
	explicit FScene(const TArray<TSharedPtr<ISceneObject>>& InObjects);

public: // Destructor

	virtual ~FScene();

public: // Static constructions

	FORCEINLINE static TSharedPtr<FScene> Create()
	{ return MakeShareable(new FScene()); }

	FORCEINLINE static TSharedPtr<FScene> Create(const TArray<TSharedPtr<ISceneObject>>& Objects)
	{ return MakeShareable(new FScene(Objects)); }

public: // Count

	FORCEINLINE uint16 GetNumOfObjects() const { return (uint16)Objects.size(); }
	
public: // Add

	int32 AddObject(const TSharedPtr<ISceneObject>& InObject);
	void AddObjects(const TArray<TSharedPtr<ISceneObject>>& InObjects);

public: // Get

	TSharedPtr<ISceneObject> GetObjectByIdx(uint16 Idx) const;

public: // Remove

	bool RemoveObjectByIdx(uint16 Idx);

public: // Draw

	void Draw(const TSharedPtr<FShaderProgram>& Shader, const TSharedPtr<FCamera>& Camera);

private: // Fields

	uint16 Counter;
	TFastMap<uint16, TSharedPtr<ISceneObject>> Objects;
};

typedef TSharedPtr<class FScene> FScenePtr;