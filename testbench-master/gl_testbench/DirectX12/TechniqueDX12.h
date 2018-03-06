#pragma once
#include "../Technique.h"
#include <wrl/client.h>
#include <d3d12.h>



class TechniqueDX12 : public Technique
{
public:
	TechniqueDX12(std::shared_ptr<Material> m, std::shared_ptr<RenderState> r);
	~TechniqueDX12();
	void enable(Renderer* renderer);
	void setCommandList(ID3D12GraphicsCommandList *cmdList);

private:

};