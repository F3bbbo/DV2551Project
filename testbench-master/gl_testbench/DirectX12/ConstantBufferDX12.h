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
#include "ResourceDX12.h"

class ConstantBufferDX12 : public ConstantBuffer
{
public:
	ConstantBufferDX12(ID3D12Device* device, std::string NAME, unsigned int location, ID3D12GraphicsCommandList* cmdList, Rootsignature* rs);
	~ConstantBufferDX12();
	void setData(const void* data, size_t size, Material* m, unsigned int location);
	void setCommandlist(ID3D12GraphicsCommandList* cmdList);
	void bind();
private:
	Rootsignature * pRS;
	ID3D12GraphicsCommandList* commandList;
	ID3D12Device* device;
	void* gConstantBufferCPU_mappedPtr;
	Resource* constantBuffer;
	std::string name;
	unsigned int location;
	void* buff = nullptr;
	void* lastMat;
	unsigned int size;
};