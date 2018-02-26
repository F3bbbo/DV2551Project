#include "MeshReader.h"
#include <windows.h>
#include <string>

bool MeshReader::MeshLoader(std::string MeshFileName,unsigned int Pflag)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(MeshFileName.c_str(), Pflag);
	if (!scene)
	{
		std::string err = "Assimp Load Error: " + std::string(importer.GetErrorString()) + "\n";
		OutputDebugStringA(err.c_str());
		return false;
	}
	return true;
}

bool MeshReader::MeshLoader(std::string MeshFileName)
{
	return MeshLoader(MeshFileName, aiProcess_Triangulate);
}

MeshReader::MeshReader()
{

}

MeshReader::~MeshReader()
{
	
}
