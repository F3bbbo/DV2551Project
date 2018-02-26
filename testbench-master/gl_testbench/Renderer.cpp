#include "DirectX12/Directx12Renderer.h"
#include "Renderer.h"


Renderer* Renderer::makeRenderer(BACKEND option)
{
	if (option == BACKEND::DX12)
		return new DirectX12Renderer();

}

