#include "d3dUtil.h"

Microsoft::WRL::ComPtr<ID3D12Resource> d3dUtil::CreateDefaultBuffer(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* cmdList,
	const void* initData, UINT64 byteSize,
	Microsoft::WRL::ComPtr<ID3D12Resource> &uploadBuffer)
{
	Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;
	//Create the actual default buffer resource
	DX::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

	//In order to copy CPU memory data into our default buffer,
	//we need to create an intermediate upload heap.
	DX::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

	//Describe the data we want to copy into the default buffer.
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;
	// Schedule to copy the data to the default bufferresource.
	// At a high level, the helper function UpdateSubresources
	// will copy the CPU memory into the intermediate upload heap.
	// Then, using ID3D12CommandList::CopySubresourceRegion,
	// the intermediate upload heap data will be copied to mBuffer.

	cmdList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources(cmdList,
		defaultBuffer.Get(),
		uploadBuffer.Get(),
		0, 0, 1, &subResourceData);

	return defaultBuffer;
}
Microsoft::WRL::ComPtr<ID3D12Resource> d3dUtil::CreateBuffer(ID3D12Device* device, UINT64 byteSize, D3D12_HEAP_TYPE type)
{
	Microsoft::WRL::ComPtr<ID3D12Resource> buffer;
	//Create the actual default buffer resource
	DX::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(type),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(buffer.GetAddressOf())));
	return buffer;
}

Microsoft::WRL::ComPtr<ID3D12Resource> d3dUtil::CreateDefaultBuffer(ID3D12Device* device, UINT64 byteSize)
{
	return CreateBuffer(device, byteSize, D3D12_HEAP_TYPE_DEFAULT);
}


Microsoft::WRL::ComPtr<ID3DBlob> d3dUtil::CompileShader(
	const std::wstring& filename,
	const D3D_SHADER_MACRO*  defines,
	const std::string& entrypoint,
	const std::string& target)
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> error;

	hr = D3DCompileFromFile(filename.c_str(), defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(),
		target.c_str(), compileFlags, 0, byteCode.GetAddressOf(), &error);

	if (error != nullptr)
		OutputDebugStringA((char*)error->GetBufferPointer());
	DX::ThrowIfFailed(hr);

	return byteCode;
}

void d3dUtil::SetResourceTransitionBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource,
	D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter)
{
	D3D12_RESOURCE_BARRIER barrierDesc = {};

	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierDesc.Transition.pResource = resource;
	barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrierDesc.Transition.StateBefore = StateBefore;
	barrierDesc.Transition.StateAfter = StateAfter;

	commandList->ResourceBarrier(1, &barrierDesc);
}

UINT d3dUtil::CalcConstantBufferByteSize(UINT byteSize)
{
	//Constant buffers must be a multiple of the minimum hardware
	//allocation size (usually 256 bytes)
	//adding 255 then masking it off
	return (byteSize + 255) & ~255;
}