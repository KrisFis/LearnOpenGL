
#pragma once

#include "LightBase.h"

class FDirectionalLight : public ILight
{
private: // Constructors

	explicit FDirectionalLight();

public: // Destructor

	virtual ~FDirectionalLight() override;
	
public: // Static constructions

	FORCEINLINE static TSharedPtr<FDirectionalLight> Create()
	{ return MakeShareable(new FDirectionalLight()); }

public: // ILight overrides

	FORCEINLINE virtual bool IsInitialized() const override { return bIsInitialized; }

public: // Getters

	FORCEINLINE const glm::vec3& GetDirection() const { return Direction; }
	
public: // Setters

	FORCEINLINE void SetDirection(const glm::vec3& Value) { Direction = Value; } 

private: // Fields

	glm::vec3 Direction;

private: // Primitive fields

	bool bIsInitialized;
};

typedef TSharedPtr<FDirectionalLight> FDirectionalLightPtr;