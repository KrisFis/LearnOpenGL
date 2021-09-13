
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
			
			if(Textures.size() > 0)
				resultMesh->SetTextures(Textures);
				
			return resultMesh;
		}
	}

	FMeshPtr ConstructCube(const std::vector<FTexture>& Textures)
	{
		return NPrivate::ConstructShape("Models/Primitive/Cube/Cube.obj", Textures);
	}

	FMeshPtr ConstructPlane(const std::vector<FTexture>& Textures)
	{
		return NPrivate::ConstructShape("Models/Primitive/Plane/Plane.obj", Textures);
	}

	FMeshPtr ConstructSphere(const std::vector<FTexture>& Textures)
	{
		return NPrivate::ConstructShape("Models/Primitive/Sphere/Sphere.obj", Textures);
	}

	FMeshPtr ConstructCylinder(const std::vector<FTexture>& Textures)
	{
		return NPrivate::ConstructShape("Models/Primitive/Cylinder/Cylinder.obj", Textures);
	}
}
