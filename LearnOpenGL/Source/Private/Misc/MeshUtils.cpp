
#include "MeshUtils.h"
#include "FileUtils.h"
#include "Model.h"
#include "Texture.h"

namespace NMeshUtils
{
	namespace NPrivate
	{
		FMeshPtr ConstructShape(const char* PathFromContent, const std::vector<FTexture>& Textures)
		{
			FModel resultModel(NFileUtils::ContentPath(PathFromContent).c_str());
			FMeshPtr resultMesh = resultModel.GetMeshes()[0];
			resultMesh->SetIsOwned(false);
			
			if(Textures.size() > 0)
				resultMesh->SetTextures(Textures);
				
			return resultMesh;
		}
	}

	FSceneObjectPtr AsScene(const FModelPtr& Model)
	{
		return (bool)Model ? std::static_pointer_cast<ISceneObject>(Model) : nullptr;
	}

	FSceneObjectPtr AsScene(const FMeshPtr& Mesh)
	{
		return (bool)Mesh ? std::static_pointer_cast<ISceneObject>(Mesh) : nullptr;
	}

	FMeshPtr ConstructCube(const FTexture Texture)
	{
		return NPrivate::ConstructShape("Models/Primitive/Cube/Cube.obj", {Texture});
	}

	FMeshPtr ConstructPlane(const FTexture Texture)
	{
		return NPrivate::ConstructShape("Models/Primitive/Plane/Plane.obj", {Texture});
	}

	FMeshPtr ConstructSphere(const FTexture Texture)
	{
		return NPrivate::ConstructShape("Models/Primitive/Sphere/Sphere.obj", {Texture});
	}

	FMeshPtr ConstructCylinder(const FTexture Texture)
	{
		return NPrivate::ConstructShape("Models/Primitive/Cylinder/Cylinder.obj", {Texture});
	}
}
