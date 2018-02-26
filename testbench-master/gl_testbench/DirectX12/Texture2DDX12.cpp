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
	if (texture != nullptr)
		delete texture;

	texture = new Resource(device, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		CD3DX12_RESOURCE_DESC::Tex2D(imageFormat, width, height), D3D12_RESOURCE_STATE_COPY_DEST);

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

	device->CreateShaderResourceView(texture->getResource(), &SRVDesc, textureDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	texture->updateResource(commandList, rgb, 4 * width, 4 * width * height);

	texture->updateState(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	return 0;
}

Texture2DDX12::Texture2DDX12(ID3D12Device* device, ID3D12GraphicsCommandList* list, Rootsignature* rs)
{
	imageFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	this->device = device;
	commandList = list;
	pRS = rs;
	texture = nullptr;
}

Texture2DDX12::~Texture2DDX12()
{
	delete texture;
}

void Texture2DDX12::bind(unsigned int slot)
{
	pRS->bindTexture(slot, textureDescriptorHeap.Get());
}