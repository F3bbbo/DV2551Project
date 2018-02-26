#pragma once
#include <d3d12.h>
#include <wrl\client.h>
#include <windows.h>
#include <pch.h> //ThrowIfFailed()
#include <d3dx12.h>

//enum ResourceState
//{
//	COMMON = 0,
//	GEN_READ = 1,
//	COPY_DEST = 2,
//	COPY_SOURCE = 3,
//	VERTEX_AND_CONSTANT_BUFFER = 4,
//	PIXEL_SHADER_RESOURCE = 5,
//	NON_PIXEL_SHADER_RESOURCE = 6
//};

class Resource
{
public:
	Resource(ID3D12Device* device, D3D12_HEAP_PROPERTIES heapProperties, D3D12_RESOURCE_DESC resourceDesc, D3D12_RESOURCE_STATES resState);
	void updateResource(ID3D12GraphicsCommandList* commandList, unsigned char* data, int rowPitch, int slicePitch);
	void updateState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState);
	~Resource();
	ID3D12Resource* getResource();
	ID3D12Resource** getResourceDP();
private:
	Resource();
	D3D12_RESOURCE_STATES state;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadHeap;
};