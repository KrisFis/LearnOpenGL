
#include "MeshUtils.h"
#include "FileUtils.h"
#include "Model.h"
#include "Texture.h"

namespace NMeshUtils
{
	namespace NPrivate
	{
		FSceneObjectPtr ConstructShape(const char* PathFromContent, const std::vector<FTexture>& Textures)
		{
			FModel resultModel(NFileUtils::ContentPath(PathFromContent).c_str());
			FMeshPtr resultMesh = resultModel.GetMeshes()[0];
			resultMesh->SetIsOwned(false);
			
			if(Textures.size() > 0)
				resultMesh->SetTextures(Textures);
				
			return CastShared<ISceneObject>(resultMesh);
		}
	}

	FSceneObjectPtr ConstructCube(const FTexture Texture)
	{
		return NPrivate::ConstructShape("Models/Primitive/Cube/Cube.obj", {Texture});
	}

	FSceneObjectPtr ConstructPlane(const FTexture Texture)
	{
		return NPrivate::ConstructShape("Models/Primitive/Plane/Plane.obj", {Texture});
	}

	FSceneObjectPtr ConstructSphere(const FTexture Texture)
	{
		return NPrivate::ConstructShape("Models/Primitive/Sphere/Sphere.obj", {Texture});
	}

	FSceneObjectPtr ConstructCylinder(const FTexture Texture)
	{
		return NPrivate::ConstructShape("Models/Primitive/Cylinder/Cylinder.obj", {Texture});
	}
}
