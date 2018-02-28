#include "MeshReader.h"
#include <windows.h>
#include <string>
#include <SimpleMath.h>
#include <memory>
#include "IA.h"
#include "DirectX12/VertexBufferDX12.h"

DirectX::SimpleMath::Vector4 DXVector4(aiVector3D vec, float w)
{
	return DirectX::SimpleMath::Vector4(vec.x, vec.y, vec.z, w);
}
DirectX::SimpleMath::Vector2 DXVector2(aiVector3D vec)
{
	return DirectX::SimpleMath::Vector2(vec.x, vec.y);
}

Assimp::Importer& MeshReader::getImporter(unsigned int key)
{
	
	//If key doesn't exist create a new importer for the key.
	if (importers[key] == nullptr)
	{
		importers[key] = new Assimp::Importer();
	}
	return *importers[key];
}

void MeshReader::extractMeshes(const aiScene * aiScene, std::vector<std::shared_ptr<Mesh>>& meshes)
{
	aiMesh **aiMeshes = aiScene->mMeshes;
	//Creating Meshes
	for (unsigned int i = 0; i < aiScene->mNumMeshes; i++)
	{
		aiMesh* aiMesh = aiMeshes[i];
		std::vector<DirectX::SimpleMath::Vector4> pos;
		std::vector<DirectX::SimpleMath::Vector4> norm;
		std::vector<DirectX::SimpleMath::Vector2> texCoords;
		std::vector<int> index;

		//Extract vertex data
		for (unsigned int j = 0; j < aiMesh->mNumVertices; j++)
		{
			//Pos
			pos.push_back(DXVector4(aiMesh->mVertices[j], 1.0f));
			//Norm
			norm.push_back(DXVector4(aiMesh->mNormals[j], 0.0f));
			//TexCoords
			texCoords.push_back(DXVector2((aiMesh->mTextureCoords[0])[j]));
			//IndexBuffer
		}
		//Create indexbuffer with faces
		for (unsigned int j = 0; j < aiMesh->mNumFaces; j++)
		{
			aiFace* face = &aiMesh->mFaces[j];
			unsigned int* indices = face->mIndices;
			for (unsigned int k = 0; k < face->mNumIndices; k++)
			{
				index.push_back(indices[k]);
			}
		}
		//Create Vertex buffers
		//Position
		std::shared_ptr<VertexBuffer> vPos = renderer->makeVertexBuffer(sizeof(pos[0]) * pos.size(), VertexBuffer::DATA_USAGE::DONTCARE);
		vPos->setData(pos.data(), sizeof(pos[0]) * pos.size(), 0);
		//Normal
		std::shared_ptr<VertexBuffer> vNorm = renderer->makeVertexBuffer(sizeof(norm[0]) * norm.size(), VertexBuffer::DATA_USAGE::DONTCARE);
		vNorm->setData(norm.data(), sizeof(norm[0]) * norm.size(), 0);
		//Texture Coordinates
		std::shared_ptr<VertexBuffer> vTexCoords = renderer->makeVertexBuffer(sizeof(texCoords[0]) * texCoords.size(), VertexBuffer::DATA_USAGE::DONTCARE);
		vTexCoords->setData(texCoords.data(), sizeof(texCoords[0]) * texCoords.size(), 0);
		//Index buffer
		std::shared_ptr<VertexBuffer> vIndex = renderer->makeVertexBuffer(sizeof(index[0]) * index.size(), VertexBuffer::DATA_USAGE::DONTCARE);
		vIndex->setData(index.data(), sizeof(index[0]) * index.size(), 0);
		

		std::shared_ptr<Mesh> outMesh = renderer->makeMesh();
		outMesh->addIAVertexBufferBinding(vPos, 0, pos.size(), sizeof(pos[0]), POS);
		outMesh->addIAVertexBufferBinding(vNorm, 0, norm.size(), sizeof(norm[0]), NORM);
		outMesh->addIAVertexBufferBinding(vTexCoords, 0, texCoords.size(), sizeof(texCoords[0]), UVCOORD);
		outMesh->addIAVertexBufferBinding(vIndex, 0, index.size(), sizeof(index[0]), INDEXBUFF);

		meshes.push_back(outMesh);
	}
}

bool MeshReader::LoadFromFile(std::string MeshFileName, unsigned int key, std::vector<std::shared_ptr<Mesh>> &meshes)
{
	//Get the threads importer
	Assimp::Importer &importer = getImporter(key);
	//Import the scene from file
	const aiScene* scene = importer.ReadFile(MeshFileName.c_str(), aiProcess_Triangulate | aiProcess_GenNormals);
	//Check if successful import of file
	if (!scene)
	{
		std::string err = "Assimp Load Error: " + std::string(importer.GetErrorString()) + "\n";
		OutputDebugStringA(err.c_str());
		return false;
	}

	extractMeshes(scene, meshes);

	return true;
}

bool MeshReader::LoadFromFile(std::string MeshFileName, std::vector<std::shared_ptr<Mesh>> &meshes)
{
	//Main thread function
	return LoadFromFile(MeshFileName, 0, meshes);
}

MeshReader::MeshReader(DirectX12Renderer *renderer)
{
	this->renderer = renderer;
}

MeshReader::~MeshReader()
{
	//Delete the Assimp::Importer
	for (auto importer : importers)
	{
		delete importer.second;
	}
}
