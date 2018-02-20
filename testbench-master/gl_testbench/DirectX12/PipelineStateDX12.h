#pragma once
#include <d3d12.h>
#include <wrl\client.h>
#include <unordered_map>
#include <d3d12.h>
#include <string>
#include <iostream>

#include "MaterialDX12.h"
#include "RenderStateDX12.h"
#include "Defines.h"

class PipelineStateDX12 {

public:
	PipelineStateDX12();
	~PipelineStateDX12();
	void setRenderState(RenderStateDX12 *renderState);
	void setMaterial(MaterialDX12 *material, ID3D12RootSignature *rs);
	void setDevice(Microsoft::WRL::ComPtr<ID3D12Device> dev);
	void setCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList);
private:
	std::string makeKey();
	void setPipeLineState(ID3D12RootSignature *rs);
	void createPipelineObject(ID3D12RootSignature *rs);
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	RenderOptions currentRenderOptions;
	MaterialDX12* currentMaterial;
	std::unordered_map <std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelineStates;
};
