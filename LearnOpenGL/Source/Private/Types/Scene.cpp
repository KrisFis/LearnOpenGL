
#include "Scene.h"
#include "Model.h"
#include "Mesh.h"

FScene::FScene(const std::vector<FModelPtr>& InModels, const std::vector<FMeshPtr>& InMeshes)
	: ModelCounter(0)
	, MeshCounter(0)
{
	for(const FModelPtr& model : InModels) AddModel(model);
	for(const FMeshPtr& mesh : InMeshes) AddMesh(mesh);
}

FScene::~FScene()
{}

int32 FScene::AddModel(FModelPtr Model)
{
	if(!(bool)Model || !Model->IsValid()) return -1;
	
	Models.insert({ModelCounter, Model});
	return ModelCounter++;
}

int32 FScene::AddMesh(FMeshPtr Mesh)
{
	if(!(bool)Mesh || !Mesh->IsValid()) return -1;

	Meshes.insert({MeshCounter, Mesh});
	return MeshCounter++;
}

void FScene::Draw(FShaderProgram& Shader)
{
	if(Models.size() > 0)
	{
		for(auto& model : Models)
		{
			model.second->Draw(Shader);
		}
	}
	
	if(Meshes.size() > 0)
	{
		for(auto& mesh : Meshes)
		{
			mesh.second->Draw(Shader);
		}
	}
}

bool FScene::RemoveModelByIdx(uint16 Idx)
{
	auto foundModel = Models.find(Idx);
	if(foundModel == Models.end()) return false;
	
	Models.erase(foundModel);
	return true;
}

bool FScene::RemoveMeshByIdx(uint16 Idx)
{
	auto foundMesh = Meshes.find(Idx);
	if(foundMesh == Meshes.end()) return false;
	
	Meshes.erase(foundMesh);
	return true;
}