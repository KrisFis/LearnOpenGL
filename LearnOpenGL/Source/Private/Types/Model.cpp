
#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "ColorUtils.h"

FModel::FModel(const char* FilePath) 
	: OutlineSize(0.f)
	, OutlineColor(NColors::Transparent)
	, bIsInitialized(false)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(FilePath, aiProcess_Triangulate | aiProcess_FlipUVs);
	
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Model loading failed [" << importer.GetErrorString() << "]" << std::endl;
		return;
	}
	
	const std::string pathAsString = FilePath;
	Directory = pathAsString.substr(0, pathAsString.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);
	
	std::cout << "Model load successful [" << FilePath << "]" << std::endl;
	
	RecalculateModel();
	bIsInitialized = true;
}

void FModel::Draw(FShaderProgram& Shader) 
{
	if(!IsValid()) return;
	
	if(IsOutlined())
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}

	DrawImpl(Shader);
	
	if(IsOutlined())
	{
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x0);
		glDisable(GL_DEPTH_TEST);
	
		Shader.SetBool("useOverrideColor", true);
		Shader.SetVec4("overrideColor", OutlineColor.ToVec4());
		
		for(auto& mesh : Meshes)
		{
			FTransform tmpTransform = mesh->GetTransform();
			const glm::vec3 oldScale = tmpTransform.Scale;

			tmpTransform.Scale = oldScale * OutlineSize;
			mesh->SetTransform(tmpTransform);

			mesh->Draw(Shader);

			tmpTransform.Scale = oldScale;
			mesh->SetTransform(tmpTransform);
		}
		
		Shader.SetBool("useOverrideColor", false);
		
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glEnable(GL_DEPTH_TEST);
	}
}

void FModel::DrawImpl(FShaderProgram& Shader)
{
	Shader.SetMat4("model", CachedModel);

	for(auto& mesh : Meshes)
	{
		mesh->Draw(Shader);
	}
}

void FModel::ProcessNode(aiNode* Node, const aiScene* Scene) 
{
	Meshes.reserve(Node->mNumMeshes);
	for(uint32 i = 0; i < Node->mNumMeshes; ++i)
	{
		aiMesh* mesh = Scene->mMeshes[Node->mMeshes[i]];
		Meshes.push_back(ProcessMesh(mesh, Scene));
	}
	
	for(uint32 i = 0; i < Node->mNumChildren; ++i)
	{
		ProcessNode(Node->mChildren[i], Scene);
	}
}

std::shared_ptr<FMesh> FModel::ProcessMesh(aiMesh* Mesh, const aiScene* Scene) 
{
	// Vertices
	std::vector<FVertex> vertices;
	vertices.reserve(Mesh->mNumVertices);
	for(uint32 i = 0; i < Mesh->mNumVertices; ++i)
	{
		const aiVector3D& currVertex = Mesh->mVertices[i];
		const aiVector3D& currNormal = Mesh->mNormals[i];
		
		FVertex vertex;
		vertex.Position = { currVertex.x, currVertex.y, currVertex.z };
		vertex.Normal = { currNormal.x, currNormal.y, currNormal.z };
		
		// We want only first texture
		if(Mesh->mTextureCoords[0])
		{
			const aiVector3D& currTexture = Mesh->mTextureCoords[0][i];
			vertex.TexCoords = { currTexture.x, currTexture.y };
		}
		else
		{
			vertex.TexCoords = {0.f, 0.f};
		}
		
		vertices.push_back(vertex);
	}
	
	std::vector<uint32> indices;
	for(uint32 i = 0; i < Mesh->mNumFaces; ++i)
	{
		const aiFace& face = Mesh->mFaces[i];
		for(uint32 j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	
	std::vector<FTexture> textures;
	if(Mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = Scene->mMaterials[Mesh->mMaterialIndex];
		std::vector<FTexture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<FTexture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	
	return std::make_shared<FMesh>(vertices, indices, textures, true);
}

std::vector<FTexture> FModel::LoadMaterialTextures(aiMaterial* Material, aiTextureType Type) 
{
	const uint32 numOfTextures = Material->GetTextureCount(Type);
	const ETextureType friendlyType = ToTextureType(Type);

	std::vector<FTexture> result;
	result.reserve(numOfTextures);
	for(uint32 i = 0; i < numOfTextures; ++i)
	{
		aiString path;
		Material->GetTexture(Type, i, &path);
		std::string filePath = Directory + '/' + std::string(path.C_Str());
		
		bool skip = false;
		for(const FTexture& texture : LoadedTextures)
		{
			if(filePath == texture.GetPath())
			{
				result.push_back(texture);
				skip = true;
				break;
			}
		}
		
		if(!skip)
		{
			FTexture newTexture = {filePath.c_str(), friendlyType};
			
			result.push_back(newTexture);
			LoadedTextures.push_back(newTexture);
		}
	}

	return result;
}

void FModel::RecalculateModel()
{
	CachedModel = Transform.CalculateModelMatrix();
}