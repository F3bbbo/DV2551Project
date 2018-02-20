#include "Texture2DDX12.h"
#include "stb_image.h"
#include "d3dUtil.h"

int Texture2DDX12::loadFromFile(std::string filename)
{
	int bpp;
	unsigned char* rgb = stbi_load(filename.c_str(), &width, &height, &bpp, STBI_rgb_alpha);
	if (rgb == nullptr)
	{
		fprintf(stderr, "Error loading texture file: %s\n", filename.c_str());
		return -1;
	}

	if (isReadyForDraw) // something has been already loaded to the texture heap
	{
		//code for removing texture data on gpu goes here if we need to implement it in the future

		//important to change resource state to D3D12_RESOURCE_STATE_COPY_DEST
	}
	D3D12_SUBRESOURCE_DATA textureData;
	textureData.pData = rgb; // pointer to our image data
	textureData.RowPitch = 4 * width; // size of one row in the texture
	textureData.SlicePitch = textureData.RowPitch * height; // (RGBA)
	// Now we copy the upload buffer contents to the texture heap
	UpdateSubresources(commandList, imageTexture.Get(), uploadHeap.Get(), 0, 0, 1, &textureData);

	// transition the texture default heap to a pixel shader resource (we will be sampling from this heap in the pixel shader to get the color of pixels)
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(imageTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	isReadyForDraw = true;

	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = { };
	SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVDesc.Format = imageFormat;
	SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	// Creates a SRV on the descriptor heap at SRVHeapHandle
	device->CreateShaderResourceView(imageTexture.Get(), &SRVDesc, this->textureDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	return 0;
}

//Allocates a heap for the texture to reside in and a upload buffer, assumes that a descriptor table heap has been allocated.
//The D3D12_CPU_DESCRIPTOR_HANDLE parameter is assumed to point to where on the descripor heap the view should be saved to
Texture2DDX12::Texture2DDX12(ID3D12Device* device, ID3D12GraphicsCommandList* list, Rootsignature* rs)
{
	this->imageFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	this->device = device;
	this->commandList = list;
	this->isReadyForDraw = false; // No data/invalid data in texture
	this->pRS = rs;
	heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	
	D3D12_RESOURCE_DESC defaultDesc = CD3DX12_RESOURCE_DESC::Tex2D(imageFormat, 640, 640);
	// Creates the image heap texture
	hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &defaultDesc, D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr, IID_PPV_ARGS(imageTexture.GetAddressOf()));

	UINT64 uploadBufferSize;
	device->GetCopyableFootprints(&defaultDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);

	// now we create an upload heap to upload our texture to the GPU
	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize), // resource description for a buffer (storing the image data in this heap just to copy to the default heap)
		D3D12_RESOURCE_STATE_GENERIC_READ, // We will copy the contents from this heap to the default heap above
		nullptr,
		IID_PPV_ARGS(&uploadHeap));

	D3D12_DESCRIPTOR_HEAP_DESC textureHeapDesc;
	textureHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	textureHeapDesc.NodeMask = 1;
	textureHeapDesc.NumDescriptors = 1;
	textureHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	DX::ThrowIfFailed(device->CreateDescriptorHeap(&textureHeapDesc, IID_PPV_ARGS(textureDescriptorHeap.GetAddressOf())));

	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = imageFormat;
	SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(imageTexture.Get(), &SRVDesc, textureDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

Texture2DDX12::~Texture2DDX12()
{
}

void Texture2DDX12::bind(unsigned int slot)
{
	pRS->bindTexture(slot, textureDescriptorHeap.Get());
}