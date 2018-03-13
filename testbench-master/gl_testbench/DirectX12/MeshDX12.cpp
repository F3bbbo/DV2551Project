#include "MeshDX12.h"
#include "VertexBufferDX12.h"
#include "ConstantBufferDX12.h"
#include "TechniqueDX12.h"
#include "Texture2DDX12.h"

MeshDX12::MeshDX12()
{
	setTranslation(Vector3(0, 0, 0));
	setRotation(Vector3(0, 0, 0));
	setScale(Vector3(1, 1, 1));
}

MeshDX12::~MeshDX12()
{
	for (auto tex : textures)
		tex.second.~shared_ptr();
}

void MeshDX12::bindIAVertexBuffer(unsigned int location)
{
	const VertexBufferBind& vb = geometryBuffers[location];
	VertexBufferDX12* buff = dynamic_cast<VertexBufferDX12*>(vb.buffer.get());
	if (buff != nullptr)
		buff->bind(vb.offset, vb.numElements, vb.sizeElement, location);
	else
		std::printf("Error: Wrong type of VertexBuffer");
}

void MeshDX12::setCommandList(ID3D12GraphicsCommandList* cmdList)
{
	TechniqueDX12* tec = static_cast<TechniqueDX12*>(technique.get());
	tec->setCommandList(cmdList);

	ConstantBufferDX12* cBuff = static_cast<ConstantBufferDX12*>(WMBuffer.get());
	cBuff->setCommandlist(cmdList);

	for (auto vertexBind : geometryBuffers)
	{
		VertexBufferDX12* vBuff = static_cast<VertexBufferDX12*>(vertexBind.second.buffer.get());
		vBuff->setCommandlist(cmdList);
	}
	for (auto texture : textures)
	{
		Texture2DDX12* tex = static_cast<Texture2DDX12*>(texture.second.get());
		tex->setCommandlist(cmdList);
	}

}


