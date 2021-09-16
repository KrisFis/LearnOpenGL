
#include "Scene.h"
#include "Model.h"
#include "Mesh.h"
#include "Shader.h"

FScene::FScene()
	: ModelCounter(0)
	, MeshCounter(0)
{}

FScene::FScene(const std::vector<FModelPtr>& InModels, const std::vector<FMeshPtr>& InMeshes)
	: ModelCounter(0)
	, MeshCounter(0)
{
	AddModels(InModels);
	AddMeshes(InMeshes);
}

FScene::~FScene()
{}

void FScene::AddModels(const std::vector<FModelPtr>& InModels)
{
	for(const FModelPtr& model : InModels) AddModel(model);
}

void FScene::AddMeshes(const std::vector<FMeshPtr>& InMeshes)
{
	for(const FMeshPtr& mesh : InMeshes) AddMesh(mesh);
}

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

FModelPtr FScene::GetModelByIdx(uint16 Idx) const
{
	auto foundModel = Models.find(Idx);
	if(foundModel == Models.end()) return nullptr;
	
	return foundModel->second;
}

FMeshPtr FScene::GetMeshByIdx(uint16 Idx) const
{
	auto foundMesh = Meshes.find(Idx);
	if(foundMesh == Meshes.end()) return nullptr;
	
	return foundMesh->second;
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

void FScene::Draw(FShaderProgram& Shader)
{
	for(auto& mesh : Meshes)
	{
		mesh.second->Draw(Shader);
	}
}
