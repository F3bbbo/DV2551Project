#pragma once

#include "../Mesh.h"
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class MeshDX12 : public Mesh
{
public:
	MeshDX12();
	~MeshDX12();
	virtual void bindIAVertexBuffer(unsigned int location);
};
