#include "PipelineStateDX12.h"

PipelineStateDX12::PipelineStateDX12()
{
	currentRenderOptions = RenderOptions::DEFAULT;
	currentMaterial = nullptr;
}

PipelineStateDX12::~PipelineStateDX12()
{
}

void PipelineStateDX12::setRenderState(RenderStateDX12 * renderState)
{
	currentRenderOptions = renderState->getRenderOptions();
}

void PipelineStateDX12::setMaterial(MaterialDX12 * material, ID3D12RootSignature *rs)
{
	currentMaterial = material;
	setPipeLineState(rs);
}

void PipelineStateDX12::setDevice(Microsoft::WRL::ComPtr<ID3D12Device> dev)
{
	device = dev;
}

void PipelineStateDX12::setCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	commandList = cmdList;
}

std::string PipelineStateDX12::makeKey()
{
	return std::to_string(currentMaterial->ID) + " " + std::to_string((int)currentRenderOptions);;
}

void PipelineStateDX12::setPipeLineState(ID3D12RootSignature *rs)
{
	std::string key = makeKey();
	if (pipelineStates[key].Get() != nullptr)
	{
		//If pipelinestate exists set it
		commandList->SetPipelineState(pipelineStates[key].Get());
		commandList->SetGraphicsRootSignature(rs);
	}
	else
	{
		//Else create a pipelinestate and then set it
		createPipelineObject(rs);
		setPipeLineState(rs);
	}
}

void PipelineStateDX12::createPipelineObject(ID3D12RootSignature *rs)
{
	D3D12_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	dsDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	currentMaterial->fillPSODesc(psoDesc);
	psoDesc.pRootSignature = rs;

	if (RenderOptions::WIREFRAME & currentRenderOptions)
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	else
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;

	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.RasterizerState.FrontCounterClockwise = false;
	psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthClipEnable = true;
	psoDesc.RasterizerState.MultisampleEnable = false;
	psoDesc.RasterizerState.AntialiasedLineEnable = false;
	psoDesc.RasterizerState.ForcedSampleCount = 0;
	psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	psoDesc.BlendState.AlphaToCoverageEnable = false;
	psoDesc.BlendState.IndependentBlendEnable = false;
	psoDesc.BlendState.RenderTarget[0].BlendEnable = false;
	psoDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	psoDesc.DepthStencilState = dsDesc;
	psoDesc.DSVFormat = DEPTH_STENCIL_FORMAT;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = SWAP_CHAIN_FORMAT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;

	std::string key = makeKey();
	HRESULT hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(pipelineStates[key].GetAddressOf()));
	if (FAILED(hr))
		std::cout << "Failed to create pipeline state." << std::endl;
	return;
}
