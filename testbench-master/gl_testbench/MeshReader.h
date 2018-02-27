#pragma once
#include <assimp/Importer.hpp>
#include <assimp\cimport.h>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>


class MeshReader
{
private:

public:
void MeshLoader(std::string MeshFileName,unsigned int Pflag);
	MeshReader();
	~MeshReader();
	const aiScene* Mesh;
};