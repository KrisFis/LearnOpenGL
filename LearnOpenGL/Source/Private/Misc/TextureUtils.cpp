
#include "TextureUtils.h"

#include "Cubemap.h"

namespace NTextureUtils
{
	TFastMap<ECubemapFace, FString> GetFacesPathFromRoot(const FString& Root)
	{
		TFastMap<ECubemapFace, FString> result;
		result.reserve(6);
		
		result.insert({ ECubemapFace::Right, Root + "/RIGHT.jpg" });
		result.insert({ ECubemapFace::Left, Root + "/LEFT.jpg" });
		result.insert({ ECubemapFace::Top, Root + "/TOP.jpg" });
		result.insert({ ECubemapFace::Bottom, Root + "/BOTTOM.jpg" });
		result.insert({ ECubemapFace::Front, Root + "/FRONT.jpg" });
		result.insert({ ECubemapFace::Back, Root + "/BACK.jpg" });
		
		return result;
	}
}