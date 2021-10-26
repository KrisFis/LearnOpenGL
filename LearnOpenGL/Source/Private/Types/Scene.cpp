
#include "Scene.h"
#include "Model.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "FileUtils.h"
#include "Texture.h"
#include "../../Public/Types/Scene.h"


namespace NScene_Private
{
	template<typename FunctorType>
	void IterateSortedObjects(const TArray<FSceneObjectPtr>& Objects, const glm::vec3& CameraPos, FunctorType&& InFunctor)
	{
		TMap<float, ISceneObject*> sortedObj;
		
		for(const auto& obj : Objects)
		{
			if(!obj.IsValid()) continue;
			
			const float distance = glm::length(CameraPos - obj->GetTransform().Position);
			sortedObj[distance] = obj.Get();
		}
		
		for(TMap<float, ISceneObject*>::reverse_iterator it = sortedObj.rbegin(); it != sortedObj.rend(); ++it)
		{
			if(InFunctor(it->second)) return;
		}
	}
}

FScene::FScene()
{}

FScene::FScene(const TArray<TSharedPtr<ISceneObject>>& InObjects)
{
	AddObjects(InObjects);
}

FScene::~FScene()
{}

int32 FScene::AddObject(const TSharedPtr<ISceneObject>& InObject)
{
	if(!InObject.IsValid()) return -1;
	
	int32 emptyIdx = -1;
	for(uint16 i = 0; i < Objects.size(); ++i)
	{
		if(Objects[i] == InObject) return i;
		else if(emptyIdx == -1 && !Objects[i].IsValid())
		{
			emptyIdx = i;
		}
	}
	
	if(emptyIdx != -1)
	{
		Objects[emptyIdx] = InObject;
		return emptyIdx;
	}
	else
	{
		Objects.push_back(InObject);
		return (int32)(Objects.size() - 1);
	}
}

void FScene::AddObjects(const TArray<TSharedPtr<ISceneObject>>& InObjects, TArray<int32>* OutIndexes)
{
	if(OutIndexes)
	{
		OutIndexes->clear();
		OutIndexes->reserve(InObjects.size());
	}
	
	for(const auto& obj : InObjects)
	{
		const int32 index = AddObject(obj);
		if(OutIndexes) OutIndexes->push_back(index);
	}
}

TSharedPtr<ISceneObject> FScene::GetObjectByIdx(uint16 Idx) const
{
	if(Idx < Objects.size()) return Objects[Idx];
	else return nullptr;
}

bool FScene::RemoveObjectByIdx(uint16 Idx)
{
	if(Idx < Objects.size())
	{
		Objects[Idx].Reset();
		return true;
	}
		
	return false;
}

void FScene::Draw(const TSharedPtr <FShaderProgram>& Shader)
{
	for(const auto& obj : Objects)
	{
		obj->Draw(Shader);
	}
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