
#pragma once

#include "LightBase.h"

class FSpotLight : public ILight
{
private: // Constructors

	explicit FSpotLight();

public: // Destructor

	virtual ~FSpotLight() override;
	
public: // Static constructions

	FORCEINLINE static TSharedPtr<FSpotLight> Create()
	{ return MakeShareable(new FSpotLight()); }

public: // ILight overrides

	FORCEINLINE virtual bool IsInitialized() const override { return bIsInitialized; }

private: // Primitive fields

	bool bIsInitialized;
};

typedef TSharedPtr<FSpotLight> FSpotLightPtr;