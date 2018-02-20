#include "RenderStateDX12.h"

RenderStateDX12::RenderStateDX12()
{
	settings = RenderOptions::DEFAULT;
}

RenderStateDX12::~RenderStateDX12()
{
	
}

void RenderStateDX12::setWireFrame(bool wireFrame)
{
	//Bit-Magic Show
	if (wireFrame)
	{
		settings = (RenderOptions) (settings | RenderOptions::WIREFRAME);
	}
	else
	{
		settings = (RenderOptions) (settings & ~RenderOptions::WIREFRAME);
	}
}

void RenderStateDX12::set()
{

}

RenderOptions RenderStateDX12::getRenderOptions()
{
	return settings;
}
