#pragma once
#include "../Sampler2D.h"
#include <d3d12.h>
#include <windows.h>
#include <wrl\client.h>


class Sampler2DDX12 :
	public Sampler2D
{
public:
	Sampler2DDX12();
	~Sampler2DDX12();
	void setMagFilter(FILTER filter);
	void setMinFilter(FILTER filter);
	void setWrap(WRAPPING s, WRAPPING t);

	int magFilter, minFilter, wrapS, wrapT;
	int samplerHandler = 0;
private:
	D3D12_SAMPLER_DESC samplerDesc = {};
};

