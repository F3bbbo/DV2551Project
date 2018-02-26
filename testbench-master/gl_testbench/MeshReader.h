#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>


class MeshReader
{
private:

public:
	bool MeshLoader(std::string MeshFileName,unsigned int Pflag);
	bool MeshLoader(std::string MeshFileName);
	MeshReader();
	~MeshReader();
};