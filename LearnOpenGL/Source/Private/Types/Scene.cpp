
#include "Scene.h"
#include "Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "FileUtils.h"

namespace NScene_Private
{
	template<typename ModelsType, typename MeshesType, typename Functor>
	void IterateUnsortedObjects(const ModelsType& Models, const MeshesType& Meshes, Functor&& InFunctor)
	{
		for(auto& model : Models)
		{
			if(InFunctor((ISceneObject*)model.second.get())) return;
		}
		
		for(auto& mesh : Meshes)
		{
			if(InFunctor((ISceneObject*)mesh.second.get())) return;
		}
	}

	template<typename ModelsType, typename MeshesType, typename Functor>
	void IterateSortedObjects(const ModelsType& Models, const MeshesType& Meshes, const FCamera& Camera, Functor&& InFunctor)
	{
		std::map<float, ISceneObject*> sortedObj;
		
		for(auto& model : Models)
		{
			const float distance = glm::length(Camera.GetPosition() - model.second->GetTransform().Position);
			sortedObj[distance] = model.second.get();
		}
		
		for(auto& mesh : Meshes)
		{
			const float distance = glm::length(Camera.GetPosition() - mesh.second->GetTransform().Position);
			sortedObj[distance] = mesh.second.get();
		}
		
		for(std::map<float, ISceneObject*>::reverse_iterator it = sortedObj.rbegin(); it != sortedObj.rend(); ++it)
		{
			if(InFunctor(it->second)) return;
		}
	}
}

FScene::FScene()
		: ModelCounter(0), MeshCounter(0)
{}

FScene::FScene(const std::vector<FModelPtr>& InModels, const std::vector<FMeshPtr>& InMeshes)
		: ModelCounter(0), MeshCounter(0)
{
	AddModels(InModels);
	AddMeshes(InMeshes);
}

FScene::~FScene()
{}

void FScene::AddModels(const std::vector<FModelPtr>& InModels)
{
	for (const FModelPtr& model: InModels) AddModel(model);
}

void FScene::AddMeshes(const std::vector<FMeshPtr>& InMeshes)
{
	for (const FMeshPtr& mesh: InMeshes) AddMesh(mesh);
}

int32 FScene::AddModel(FModelPtr Model)
{
	if (!(bool) Model || !Model->IsValid()) return -1;

	Models.insert({ModelCounter, Model});
	return ModelCounter++;
}

int32 FScene::AddMesh(FMeshPtr Mesh)
{
	if (!(bool) Mesh || !Mesh->IsValid()) return -1;

	Meshes.insert({MeshCounter, Mesh});
	return MeshCounter++;
}

FModelPtr FScene::GetModelByIdx(uint16 Idx) const
{
	auto foundModel = Models.find(Idx);
	if (foundModel == Models.end()) return nullptr;

	return foundModel->second;
}

FMeshPtr FScene::GetMeshByIdx(uint16 Idx) const
{
	auto foundMesh = Meshes.find(Idx);
	if (foundMesh == Meshes.end()) return nullptr;

	return foundMesh->second;
}

bool FScene::RemoveModelByIdx(uint16 Idx)
{
	auto foundModel = Models.find(Idx);
	if (foundModel == Models.end()) return false;

	Models.erase(foundModel);
	return true;
}

bool FScene::RemoveMeshByIdx(uint16 Idx)
{
	auto foundMesh = Meshes.find(Idx);
	if (foundMesh == Meshes.end()) return false;

	Meshes.erase(foundMesh);
	return true;
}

void FScene::Draw(FShaderProgram& Shader, const FCamera& Camera)
{
	// Draw terrain first
	Meshes[0]->Draw(Shader);

	NScene_Private::IterateSortedObjects(
			Models,
			Meshes,
			Camera,
			[&](ISceneObject* Obj) -> bool
			{
				if(static_cast<FMesh*>(Obj)->GetTextures()[0].GetPath() == NFileUtils::ContentPath("Textures/Default/floor_rocks.jpg"))
					return false;
					
				Obj->Draw(Shader);
				
				return false;
			}
	);
}
