#pragma once
#include "../Technique.h"
#include <wrl/client.h>
#include <d3d12.h>



class TechniqueDX12 : public Technique
{
public:
	TechniqueDX12(Material* m, RenderState* r);
	~TechniqueDX12();
	void enable(Renderer* renderer);

private:

};