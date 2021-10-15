
#pragma once

#include "ModuleMinimal.h"

// Forward declaration
class ISceneObject;

class FScene
{
	
public: // Constructors

	FScene();
	FScene(const std::vector<FSceneObjectPtr>& InObjects);
	virtual ~FScene();
	
public: // Count

	inline uint16 GetNumOfObjects() const { return (uint16)Objects.size(); }
	
public: // Add

	int32 AddObject(const FSceneObjectPtr& InObject);
	void AddObjects(const std::vector<FSceneObjectPtr>& InObjects);

public: // Get

	FSceneObjectPtr GetObjectByIdx(uint16 Idx) const;

public: // Remove

	bool RemoveObjectByIdx(uint16 Idx);

public: // Draw

	void Draw(FShaderProgram& Shader, const FCamera& Camera);

private: // Fields

	uint16 Counter;
	std::unordered_map<uint16, FSceneObjectPtr> Objects;
};