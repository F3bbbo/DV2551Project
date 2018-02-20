#pragma once
#include "../ConstantBuffer.h"
#include <windows.h>
#include <wrl\client.h>
#include <d3d12.h>
#include <dxgi1_5.h>
#include "../Utilityfunctions.h"
#include "RootSignature.h"
#include "d3dUtil.h"
#include <DirectXMath.h>


class ConstantBufferDX12 : public ConstantBuffer
{
public:
	ConstantBufferDX12(Microsoft::WRL::ComPtr<ID3D12Device> gDevice,std::string NAME, unsigned int location, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList, Rootsignature* rs/*, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>* descriptorHeaps*/);
	~ConstantBufferDX12();
	void setData(const void* data, size_t size, Material* m, unsigned int location);
	void bind(Material*);
private:
	Rootsignature * pRS;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	void* 	gConstantBufferCPU_mappedPtr;
	Microsoft::WRL::ComPtr<ID3D12Resource>	gConstantBufferResource;
	Microsoft::WRL::ComPtr<ID3D12Resource>	upploadbuffer;
	std::string name;
	unsigned int location;
	void* buff = nullptr;
	void* lastMat;
};