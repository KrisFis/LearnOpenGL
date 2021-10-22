
#include "Scene.h"
#include "Model.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "FileUtils.h"
#include "Texture.h"

namespace NScene_Private
{
	template<typename ObjectsMapType, typename FunctorType>
	void IterateSortedObjects(const ObjectsMapType& Objects, const glm::vec3& CameraPos, FunctorType&& InFunctor)
	{
		TMap<float, ISceneObject*> sortedObj;
		
		for(auto& obj : Objects)
		{
			const float distance = glm::length(CameraPos - obj.second->GetTransform().Position);
			sortedObj[distance] = obj.second.Get();
		}
		
		for(TMap<float, ISceneObject*>::reverse_iterator it = sortedObj.rbegin(); it != sortedObj.rend(); ++it)
		{
			if(InFunctor(it->second)) return;
		}
	}
}

FScene::FScene()
		: Counter(0)
{}

FScene::FScene(const TArray<TSharedPtr<ISceneObject>>& InObjects)
	: Counter(0)
{
	AddObjects(InObjects);
}

FScene::~FScene()
{}

int32 FScene::AddObject(const TSharedPtr<ISceneObject>& InObject)
{
	if(!InObject.IsValid()) return -1;
	
	for(const auto& obj : Objects)
	{
		if(obj.second == InObject) return obj.first;
	}
	
	Objects.insert({Counter++, InObject});
	
	return true;
}

void FScene::AddObjects(const TArray<TSharedPtr<ISceneObject>>& InObjects)
{
	for(const auto& obj : InObjects) AddObject(obj);
}

TSharedPtr<ISceneObject> FScene::GetObjectByIdx(uint16 Idx) const
{
	auto foundObj = Objects.find(Idx);
	if(foundObj == Objects.end())
		return nullptr;
	
	return foundObj->second;
}

bool FScene::RemoveObjectByIdx(uint16 Idx)
{
	auto foundObj = Objects.find(Idx);
	if(foundObj == Objects.end())
		return false;
		
	Objects.erase(foundObj);
		
	return true;
}

void FScene::Draw(const TSharedPtr<FShaderProgram>& Shader, const TSharedPtr<FCamera>& Camera)
{
	// Draw terrain first
	Objects[0]->Draw(Shader);

	NScene_Private::IterateSortedObjects(
			Objects,
			Camera->GetPosition(),
			[&](ISceneObject* Obj) -> bool
			{
				if(static_cast<FMesh*>(Obj)->GetTextures()[0]->GetPath() == NFileUtils::ContentPath("Textures/Default/floor_rocks.jpg"))
					return false;
					
				Obj->Draw(Shader);
				
				return false;
			}
	);
}