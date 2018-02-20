#pragma once

#include "../VertexBuffer.h"
#include "wrl/client.h"

#include <d3d12.h>

class VertexBufferDX12 : public VertexBuffer
{
public:
	VertexBufferDX12(ID3D12Device *device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &cmdList);
	~VertexBufferDX12();
	virtual void setData(const void* data, size_t size, size_t offset);
	virtual void bind(size_t offset, size_t size, unsigned int location);
	virtual void bind(size_t offset, size_t numElements, size_t elementSize, unsigned int location);
	virtual void unbind();
	virtual size_t getSize();
	void createBuffer(ID3D12Device *device, size_t size);
private:
	//Pointers
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12Resource> bufferGPU;
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
	//Resource View
	D3D12_VERTEX_BUFFER_VIEW buffView;
	size_t byteSize;
	unsigned int location;
};
