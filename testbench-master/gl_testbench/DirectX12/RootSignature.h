#pragma once
#include <wrl\client.h>
#include <d3d12.h>
#include <iostream>
#include <vector>

#define CONST_BUFF_OFFSET 0
#define NUM_CONST_BUFF 8

#define SRV_OFFSET NUM_CONST_BUFF
#define NUM_SRV 8

#define STRUCT_BUFF_OFFSET SRV_OFFSET + NUM_SRV 
#define NUM_STRUCT_BUFF 8


class Rootsignature
{

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature>  gRootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;

public:
	void bindConstantBuffer(int slot, ID3D12Resource* resource);
	void bindTexture(int slot, ID3D12DescriptorHeap* descHeap);
	void bindStructuredBuffers(int slot, ID3D12Resource* resource);
	void bindStructuredBuffers(int slot, ID3D12Resource* resource, size_t offset);
	//void executeBind(ID3D12GraphicsCommandList *cmdList);
	//void clearBinds();
	std::vector<ID3D12DescriptorHeap*> descriptorHeaps; // Store all heaps in a vector such that they can be bound once
	std::vector<UINT> slots;
	Rootsignature();
	~Rootsignature();
	void setCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> &commandList);
	void CreateRootsignature(Microsoft::WRL::ComPtr<ID3D12Device> gDevice);
	void bindRootSignature();
	ID3D12RootSignature * getRootSignature();
	void setRootTableData();

};

