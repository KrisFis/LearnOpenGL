
#include "MeshUtils.h"
#include "FileUtils.h"
#include "Model.h"
#include "Mesh.h"
#include "Texture.h"

namespace NMeshUtils
{
	namespace
	{
		FSceneObjectPtr ConstructShape(const char* PathFromContent, const TArray<TSharedPtr<FTexture>>& Textures)
		{
			FMeshPtr resultMesh = FModel::Create(NFileUtils::ContentPath(PathFromContent).c_str())->GetMeshes()[0];
			resultMesh->SetIsOwned(false);
			
			if(Textures.size() > 0)
				resultMesh->SetTextures(Textures);
				
			return CastShared<ISceneObject>(resultMesh);
		}
	}

	FSceneObjectPtr ConstructCube(const TArray<TSharedPtr<FTexture>> Textures)
	{
		return ConstructShape("Models/Primitive/Cube/Cube.obj", Textures);
	}

	FSceneObjectPtr ConstructPlane(const TArray<TSharedPtr<FTexture>> Textures)
	{
		return ConstructShape("Models/Primitive/Plane/Plane.obj", Textures);
	}

	FSceneObjectPtr ConstructSphere(const TArray<TSharedPtr<FTexture>> Textures)
	{
		return ConstructShape("Models/Primitive/Sphere/Sphere.obj", Textures);
	}

	FSceneObjectPtr ConstructCylinder(const TArray<TSharedPtr<FTexture>> Textures)
	{
		return ConstructShape("Models/Primitive/Cylinder/Cylinder.obj", Textures);
	}
}
