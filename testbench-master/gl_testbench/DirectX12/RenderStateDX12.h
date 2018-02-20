#pragma once
#include "../RenderState.h"
#include <d3d12.h>
#include <d3dx12.h>
#include <WinBase.h>
#include <wrl\client.h>

enum RenderOptions
{
	DEFAULT = 0,
	WIREFRAME = 1
};


class RenderStateDX12 : public RenderState
{
private:
	RenderOptions settings;
public:
	RenderStateDX12();
	~RenderStateDX12();
	void setWireFrame(bool wireFrame);
	void set();
	RenderOptions getRenderOptions();
};
