
#include "TextureUtils.h"

#include "Cubemap.h"

namespace NTextureUtils
{
	TFastMap<ECubemapFace, FString>
	NTextureUtils::GetFacesPathFromRoot(const FString& Root, const FString& Extension, bool UseIndexes)
	{
		TFastMap<ECubemapFace, FString> result;
		result.reserve(6);
		
		if(!UseIndexes)
		{
			result.insert({ ECubemapFace::Right, Root + "/RIGHT." + Extension });
			result.insert({ ECubemapFace::Left, Root + "/LEFT." + Extension });
			result.insert({ ECubemapFace::Top, Root + "/TOP." + Extension });
			result.insert({ ECubemapFace::Bottom, Root + "/BOTTOM." + Extension });
			result.insert({ ECubemapFace::Front, Root + "/FRONT." + Extension });
			result.insert({ ECubemapFace::Back, Root + "/BACK." + Extension });
		}
		else
		{
			result.insert({ ECubemapFace::Right, Root + "/1." + Extension });
			result.insert({ ECubemapFace::Left, Root + "/2." + Extension });
			result.insert({ ECubemapFace::Top, Root + "/3." + Extension });
			result.insert({ ECubemapFace::Bottom, Root + "/4." + Extension });
			result.insert({ ECubemapFace::Front, Root + "/5." + Extension });
			result.insert({ ECubemapFace::Back, Root + "/6." + Extension });
		}
		
		return result;
	}
}