#pragma once
#ifndef D3DUTIL_H
#define D3DUTIL_H
//DirectX 12
#include <d3d12.h>
//Dx12 Helper Lib
#include <d3dx12.h>
#include <dxgi1_4.h>
//Include for error checking
#include <pch.h> //ThrowIfFailed()
//ComPtr
#include <wrl/client.h>
//Strings
#include <string>
//Compiler
#include <d3dcompiler.h>

namespace d3dUtil 
{
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData, UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource> &uploadBuffer);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(ID3D12Device* device, UINT64 byteSize, D3D12_HEAP_TYPE type);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device* device, UINT64 byteSize);


	Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO*  defines,
		const std::string& entrypoint,
		const std::string& target);

	void SetResourceTransitionBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource,
		D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);

	UINT CalcConstantBufferByteSize(UINT byteSize);

}
#endif
