
#pragma once

#include "LightBase.h"

class FPointLight : public ILight
{
private: // Constructors

	explicit FPointLight();

public: // Destructor

	virtual ~FPointLight() override;
	
public: // Static constructions

	FORCEINLINE static TSharedPtr<FPointLight> Create()
	{ return MakeShareable(new FPointLight()); }

public: // ILight overrides

	FORCEINLINE virtual bool IsInitialized() const override { return bIsInitialized; }

private: // Primitive fields

	bool bIsInitialized;
};

typedef TSharedPtr<FPointLight> FPointLightPtr;