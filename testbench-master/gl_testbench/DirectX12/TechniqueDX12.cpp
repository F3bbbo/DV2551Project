#include "TechniqueDX12.h"
#include <d3dx12.h>
#include "RenderStateDX12.h"
#include "Directx12Renderer.h"
#include "MaterialDX12.h"

TechniqueDX12::TechniqueDX12(std::shared_ptr<Material> m, std::shared_ptr<RenderState> r)
	:Technique(m, r)
{

}

TechniqueDX12::~TechniqueDX12()
{

}

void TechniqueDX12::enable(Renderer * renderer)
{
	renderer->setRenderState(renderState.get());
	MaterialDX12* pMaterialDX12 = dynamic_cast<MaterialDX12*>(material.get());
	if (pMaterialDX12 != nullptr)
	{
		pMaterialDX12->enable(renderer);
	}
	else
	{
		printf("Error: Not a MaterialDX12 in TechniqueDX12::enable()");
	}
}

void TechniqueDX12::setCommandList(ID3D12GraphicsCommandList * cmdList)
{
	MaterialDX12* mat = dynamic_cast<MaterialDX12*>(material.get());
	if (mat == nullptr)
	{
		OutputDebugStringA("Error: Material not of type MaterialDX12.");
	}
	else
	{
		mat->setCommandlist(cmdList);
	}
}
