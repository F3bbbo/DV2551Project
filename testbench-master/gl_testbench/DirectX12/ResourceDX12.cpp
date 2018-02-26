#include "ResourceDX12.h"

Resource::Resource(ID3D12Device* device, D3D12_HEAP_PROPERTIES heapProperties, D3D12_RESOURCE_DESC resourceDesc, D3D12_RESOURCE_STATES resState)
{
	DX::ThrowIfFailed(device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, resState,
		nullptr, IID_PPV_ARGS(&resource)));

	UINT64 uploadBufferSize;
	device->GetCopyableFootprints(&resourceDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);

	// Create a generic upload buffer
	DX::ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadHeap)));

	state = resState;
}

void Resource::updateResource(ID3D12GraphicsCommandList* commandList, unsigned char * data, int rowPitch, int slicePitch)
{
	D3D12_SUBRESOURCE_DATA textureData;
	textureData.pData = data; // pointer to our image data
	textureData.RowPitch = rowPitch; // size of one row in the texture
	textureData.SlicePitch = slicePitch;
	
	UpdateSubresources(commandList, resource.Get(), uploadHeap.Get(), 0, 0, 1, &textureData);
}

void Resource::updateState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState)
{
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), state, newState));
	state = newState;
}

Resource::~Resource()
{
}



ID3D12Resource * Resource::getResource()
{
	return resource.Get();
}

ID3D12Resource ** Resource::getResourceDP()
{
	return resource.GetAddressOf();
}
