
#pragma once

#include "ModuleMinimal.h"

namespace NFileUtils 
{
	FORCEINLINE FString ContentPath() { return "../../../Content"; } 
	FORCEINLINE FString ContentPath(const FString& FilePath) { return ContentPath() + '/' + FilePath; }
	FORCEINLINE FString ContentPath(const char* FilePath) { return ContentPath() + '/' + FString(FilePath); }
}