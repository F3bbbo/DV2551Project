#pragma once

#include "../Mesh.h"
#include <SimpleMath.h>
#include <d3d12.h>

using namespace DirectX::SimpleMath;

class MeshDX12 : public Mesh
{
public:
	MeshDX12();
	~MeshDX12();
	virtual void bindIAVertexBuffer(unsigned int location);
	void setCommandList(ID3D12GraphicsCommandList* cmdList);
};
