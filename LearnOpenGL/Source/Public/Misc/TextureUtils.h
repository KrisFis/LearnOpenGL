
#pragma once

#include "ModuleMinimal.h"

enum class ECubemapFace : uint8;

namespace NTextureUtils
{
	TFastMap<ECubemapFace, FString> GetFacesPathFromRoot(const FString& Root);
}