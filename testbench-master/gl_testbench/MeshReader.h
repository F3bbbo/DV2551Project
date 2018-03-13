#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <map>
#include "DirectX12/Directx12Renderer.h"
#include "DirectX12/MeshDX12.h"
#include "Texture2D.h"


class MeshReader
{
private:
	DirectX12Renderer * renderer;
	std::map<unsigned int, Assimp::Importer*> importers;
	Assimp::Importer& getImporter(unsigned int key);
	void extractMeshes(const aiScene* scene, std::vector<std::shared_ptr<Mesh>> &meshes, std::shared_ptr<Texture2D> texture, unsigned int key);
public:
	bool LoadFromFile(std::string MeshFileName, std::string TextureFileName, std::vector<std::shared_ptr<Mesh>> &meshes, unsigned int key);
	bool LoadFromFile(std::string MeshFileName, std::string TextureFileName, std::vector<std::shared_ptr<Mesh>> &meshes);
	MeshReader(DirectX12Renderer *renderer);
	~MeshReader();
};