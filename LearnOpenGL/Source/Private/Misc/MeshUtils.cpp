
#include "MeshUtils.h"
#include "FileUtils.h"
#include "Model.h"
#include "Texture.h"

namespace NMeshUtils
{
	FMeshPtr ConstructCube(const std::vector<FTexture>& Textures)
	{
		FModel resultModel(NFileUtils::ContentPath("Models/Primitive/Cube/Cube.obj").c_str());
		FMeshPtr resultMesh = resultModel.GetMeshes()[0];
		
		if(Textures.size() > 0)
			resultMesh->SetTextures(Textures);
			
		return resultMesh;
	}

	FMeshPtr ConstructPlane(const std::vector<FTexture>& Textures)
	{
		FModel resultModel(NFileUtils::ContentPath("Models/Primitive/Plane/Plane.obj").c_str());
		FMeshPtr resultMesh = resultModel.GetMeshes()[0];
		
		if(Textures.size() > 0)
			resultMesh->SetTextures(Textures);
			
		return resultMesh;
	}

	FMeshPtr ConstructSphere(const std::vector<FTexture>& Textures)
	{
		FModel resultModel(NFileUtils::ContentPath("Models/Primitive/Sphere/Sphere.obj").c_str());
		FMeshPtr resultMesh = resultModel.GetMeshes()[0];
		
		if(Textures.size() > 0)
			resultMesh->SetTextures(Textures);
			
		return resultMesh;
	}
}
