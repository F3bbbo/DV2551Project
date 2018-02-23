#include "VertexBufferDX12.h"
#include "d3dUtil.h"
#include "pch.h"
#include <d3dx12.h>


VertexBufferDX12::VertexBufferDX12(ID3D12Device *device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList, Rootsignature* rs)
{
	this->commandList = cmdList;
	this->rootSignature = rs;
	byteSize = 0;
	location = UINT_MAX;
	bufferGPU = nullptr;
	uploadBuffer = nullptr;
}

VertexBufferDX12::~VertexBufferDX12()
{
	
}

void VertexBufferDX12::setData(const void * data, size_t size, size_t offset)
{
	//Unsure code (found no example and haven't been able to try it)
	void* mapData = nullptr;
	D3D12_RANGE range = { 0, 0 };
	uploadBuffer->Map(0, &range, &mapData);
	memcpy(mapData, data, size);
	uploadBuffer->Unmap(0, &range);
	//Upload buffer
	d3dUtil::SetResourceTransitionBarrier(
		commandList.Get(),
		bufferGPU.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	commandList->CopyBufferRegion(bufferGPU.Get(), offset, uploadBuffer.Get(), 0, size);
	d3dUtil::SetResourceTransitionBarrier(
		commandList.Get(),
		bufferGPU.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_COMMON
	);
}

void VertexBufferDX12::bind(size_t offset, size_t size, unsigned int location)
{
	rootSignature->bindStructuredBuffers(location, bufferGPU.Get(), offset);
	
}

void VertexBufferDX12::bind(size_t offset, size_t numElements, size_t elementSize, unsigned int location)
{
	bind(offset, numElements * elementSize, location);
	////Set current location
	//this->location = location;
	////Set buffer view
	//buffView.BufferLocation = bufferGPU->GetGPUVirtualAddress() + offset;
	//buffView.SizeInBytes = elementSize * numElements;
	//buffView.StrideInBytes = elementSize;
	////Bind buffer
	//commandList->IASetVertexBuffers(location, 1, &buffView);
}

void VertexBufferDX12::unbind()
{
	//commandList->IASetVertexBuffers(location, 1, nullptr);
}

size_t VertexBufferDX12::getSize()
{
	return byteSize;
}

void VertexBufferDX12::createBuffer(ID3D12Device *device, size_t size)
{
	byteSize = size;
	bufferGPU = d3dUtil::CreateDefaultBuffer(device, byteSize);
	
	DX::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size), D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(&uploadBuffer)));
}
