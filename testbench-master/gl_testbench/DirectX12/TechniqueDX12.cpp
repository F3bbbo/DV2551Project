#include "TechniqueDX12.h"
#include <d3dx12.h>
#include "RenderStateDX12.h"
#include "Directx12Renderer.h"
#include "MaterialDX12.h"

TechniqueDX12::TechniqueDX12(Material* m, RenderState* r)
	:Technique(m, r)
{

}

TechniqueDX12::~TechniqueDX12()
{

}

void TechniqueDX12::enable(Renderer * renderer)
{
	renderer->setRenderState(renderState);
	MaterialDX12* pMaterialDX12 = dynamic_cast<MaterialDX12*>(material);
	if (pMaterialDX12 != nullptr)
	{
		pMaterialDX12->enable(renderer);
	}
	else
	{
		printf("Error: Not a MaterialDX12 in TechniqueDX12::enable()");
	}
}
