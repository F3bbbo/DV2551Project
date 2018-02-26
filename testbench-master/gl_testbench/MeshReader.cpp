#include "MeshReader.h"

void MeshReader::MeshLoader(std::string MeshFileName,unsigned int Pflag)
{

	Mesh=aiImportFile(MeshFileName.c_str(), Pflag);
}

MeshReader::MeshReader()
{

}

MeshReader::~MeshReader()
{

}
