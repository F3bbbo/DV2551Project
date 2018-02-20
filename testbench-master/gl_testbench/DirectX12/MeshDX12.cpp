#include "MeshDX12.h"
#include "VertexBufferDX12.h"

MeshDX12::MeshDX12()
{
}

MeshDX12::~MeshDX12()
{
}

void MeshDX12::bindIAVertexBuffer(unsigned int location)
{
	const VertexBufferBind& vb = geometryBuffers[location];
	VertexBufferDX12* buff = dynamic_cast<VertexBufferDX12*>(vb.buffer);
	if (buff != nullptr)
		buff->bind(vb.offset, vb.numElements, vb.sizeElement, location);
	else
		std::printf("Error: Wrong type of VertexBuffer");
}


