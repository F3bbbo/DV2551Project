#pragma once
#include "..\Texture2D.h"
#include <string>
#include <d3d12.h>
#include <wrl\client.h>
#include <windows.h>
#include "RootSignature.h" // For defines
#include "ResourceDX12.h"

class Texture2DDX12 : public Texture2D
{
public:
	int loadFromFile(std::string filename);
	Texture2DDX12(ID3D12Device* device, ID3D12GraphicsCommandList* list, Rootsignature* rs);
	~Texture2DDX12();
	void setCommandlist(ID3D12GraphicsCommandList* cmdList);
	// bind texture to be used in the pipeline, binding to
	// slot "slot" in the active fragment shader.
	// slot can have different interpretation depending on the API.
	void bind(unsigned int slot);
	void setState(D3D12_RESOURCE_STATES state);
	void setBindState();

private:
	bool isReadyForDraw;
	DXGI_FORMAT imageFormat;
	int width, height;
	unsigned int location;
	ID3D12Device* device;
	ID3D12GraphicsCommandList* commandList;
	Resource* texture;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> textureDescriptorHeap;
	Rootsignature* pRS;
	HRESULT hr; //debugger
};