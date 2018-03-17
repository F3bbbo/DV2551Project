#include "ConstantBufferDX12.h"

ConstantBufferDX12::ConstantBufferDX12(ID3D12Device* device, std::string NAME, unsigned int location, 
	ID3D12GraphicsCommandList* cmdList, Rootsignature* rs)
{
	pRS = rs;//setting root signature
	this->commandList = cmdList;
	this->location = location;
	this->device = device;
	size = 0;
}

ConstantBufferDX12::~ConstantBufferDX12()
{
	delete constantBuffer;
}

void ConstantBufferDX12::setData(const void * data, size_t inSize, Material * m, unsigned int location)
{
	size_t size = d3dUtil::CalcConstantBufferByteSize(inSize);
	if (this->size < size)
	{
		this->size = size;
		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.CreationNodeMask = 1; //used when multi-gpu
		heapProperties.VisibleNodeMask = 1; //used when multi-gpu
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Width = size;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		constantBuffer = new Resource(device, heapProperties, resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST);

		const char * tmpName = name.c_str();
		constantBuffer->getResource()->SetName(LPCtransferfromstring(tmpName));
	}
	else
	{
		constantBuffer->updateState(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
	}

	constantBuffer->updateResource(commandList, data, size, size);
	constantBuffer->updateState(commandList, D3D12_RESOURCE_STATE_COMMON);
	this->location = location;
	//D3D12_RANGE range = { 0, 0 }; //We do not intend to read this resource on the CPU.
	//upploadbuffer->Map(0, &range, &gConstantBufferCPU_mappedPtr);

	//memcpy(gConstantBufferCPU_mappedPtr, data, size);
	//upploadbuffer->Unmap(0, &range);

	/*d3dUtil::SetResourceTransitionBarrier(
		commandList.Get(),
		gConstantBufferResource.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST
	);*/
	/*commandList->CopyBufferRegion(gConstantBufferResource.Get(), 0, upploadbuffer.Get(), 0, size);
	d3dUtil::SetResourceTransitionBarrier(
		commandList.Get(),
		gConstantBufferResource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_COMMON
	);*/
}

void ConstantBufferDX12::bind()
{
	pRS->bindConstantBuffer(location, constantBuffer->getResource());
}

void ConstantBufferDX12::setCommandlist(ID3D12GraphicsCommandList * cmdList)
{
	commandList = cmdList;
}


