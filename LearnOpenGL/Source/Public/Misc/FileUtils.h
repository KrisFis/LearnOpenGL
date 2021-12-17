
#pragma once

#include "ModuleMinimal.h"

namespace NFileUtils 
{
	FORCEINLINE static FString ContentPath() { return "../../../Content"; } 
	FORCEINLINE static FString ContentPath(const FString& FilePath) { return ContentPath() + '/' + FilePath; }
	FORCEINLINE static FString ContentPath(const char* FilePath) { return ContentPath() + '/' + FString(FilePath); }
}