
#pragma once

#include <string>

namespace NFileUtils 
{
	inline std::string ContentPath() { return "../../../Content"; } 
	inline std::string ContentPath(const char* FilePath) { return ContentPath() + '/' + std::string(FilePath); }
}