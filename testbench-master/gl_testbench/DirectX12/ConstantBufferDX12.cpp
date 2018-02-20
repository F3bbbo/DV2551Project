#include "ConstantBufferDX12.h"

ConstantBufferDX12::ConstantBufferDX12(
	Microsoft::WRL::ComPtr<ID3D12Device> gDevice,std::string NAME, 
	unsigned int location, 
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList,
	Rootsignature* rs)
{
	pRS = rs;//setting root signature
	this->commandList = cmdList;
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.CreationNodeMask = 1; //used when multi-gpu
	heapProperties.VisibleNodeMask = 1; //used when multi-gpu
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = 1024;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


	DX::ThrowIfFailed(gDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(1024*64), D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(&upploadbuffer)));

	gDevice->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&gConstantBufferResource));

	const char * tmpName = name.c_str();
	upploadbuffer->SetName(LPCtransferfromstring(tmpName));

	this->location = location;

}

ConstantBufferDX12::~ConstantBufferDX12()
{

}

void ConstantBufferDX12::setData(const void * data, size_t size, Material * m, unsigned int location)
{
	D3D12_RANGE range = { 0, 0 }; //We do not intend to read this resource on the CPU.
	upploadbuffer->Map(0, &range, &gConstantBufferCPU_mappedPtr);

	memcpy(gConstantBufferCPU_mappedPtr, data, size);
	upploadbuffer->Unmap(0, &range);

	d3dUtil::SetResourceTransitionBarrier(
		commandList.Get(),
		gConstantBufferResource.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST
	);
	commandList->CopyBufferRegion(gConstantBufferResource.Get(), 0, upploadbuffer.Get(), 0, size);
	d3dUtil::SetResourceTransitionBarrier(
		commandList.Get(),
		gConstantBufferResource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_COMMON
	);
	this->location = location;
}

void ConstantBufferDX12::bind(Material *data)
{
	pRS->bindConstantBuffer(location, gConstantBufferResource.Get());
}
