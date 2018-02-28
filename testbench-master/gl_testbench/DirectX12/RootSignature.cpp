#include "RootSignature.h"
#include "pch.h"

void Rootsignature::bindConstantBuffer(int slot, ID3D12Resource * resource)
{
	commandList->SetGraphicsRootConstantBufferView(CONST_BUFF_OFFSET + slot, resource->GetGPUVirtualAddress());
	//addBind(slot, heap, CONSTANT_BUFFER_VIEW);
}

void Rootsignature::bindTexture(int slot, ID3D12DescriptorHeap* descHeap)
{
	descriptorHeaps.push_back(descHeap);
	slots.push_back(SRV_OFFSET + slot);
	//addBind(slot, heap, SHADER_RESOURCE_VIEW);
}

void Rootsignature::bindStructuredBuffers(int slot, ID3D12Resource * resource)
{
	bindStructuredBuffers(slot, resource, 0);
	//addBind(slot, heap, UNORDERED_ACCESS_VIEW);
}

void Rootsignature::bindStructuredBuffers(int slot, ID3D12Resource * resource, size_t offset)
{
	commandList->SetGraphicsRootShaderResourceView(STRUCT_BUFF_OFFSET + slot, resource->GetGPUVirtualAddress() + offset);
}

//void Rootsignature::executeBind(ID3D12GraphicsCommandList * cmdList)
//{
//	//Set heaps
//	cmdList->SetDescriptorHeaps(heaps.size(), heaps.data());
//	for (UINT i = 0; i < heaps.size(); i++)
//	{
//		if (bindInfo[i].type == CONSTANT_BUFFER_VIEW)
//		{
//			cmdList->SetGraphicsRootConstantBufferView(bindInfo[i].slot, heaps[i]->GetGPUDescriptorHandleForHeapStart());
//		}
//	}
//}

//void Rootsignature::clearBinds()
//{
//	bindInfo.clear();
//	heaps.clear();
//}

Rootsignature::Rootsignature()
{

}

Rootsignature::~Rootsignature()
{
}

void Rootsignature::setCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	this->commandList = commandList;
}

void Rootsignature::CreateRootsignature(Microsoft::WRL::ComPtr<ID3D12Device> gDevice)
{
	std::vector<D3D12_ROOT_PARAMETER> rootParams;
	
	//Defining Constant buffer entries
	for (UINT i = 0; i < NUM_CONST_BUFF; i++)
	{
		D3D12_ROOT_PARAMETER rp = {};
		rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rp.Descriptor = { i, 0 }; // {register, space}
		rp.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		//Push the root param on the list
		rootParams.push_back(rp);
	}

	//Defining SRV entries
	D3D12_DESCRIPTOR_RANGE dtRanges[NUM_SRV];
	for (UINT i = 0; i < NUM_SRV; i++)
	{
		//Define the range
		dtRanges[i].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		dtRanges[i].NumDescriptors = 1; //Only one view in each table
		dtRanges[i].BaseShaderRegister = i; //register(ti);
		dtRanges[i].RegisterSpace = 1; //register(b0,space1);
		dtRanges[i].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		//Define the table
		D3D12_ROOT_DESCRIPTOR_TABLE dt;
		dt.NumDescriptorRanges = 1;
		dt.pDescriptorRanges = &dtRanges[i];

		D3D12_ROOT_PARAMETER rp = {};
		rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rp.DescriptorTable = dt;
		//rp.Descriptor = { i, 0 }; // {register, space}
		rp.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		//Push the root param on the list
		rootParams.push_back(rp);
	}

	//Defining UAV entries
	for (UINT i = 0; i < NUM_STRUCT_BUFF; i++)
	{
		D3D12_ROOT_PARAMETER rp = {};
		rp.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		rp.Descriptor = { i, 0 }; // {register, space}
		rp.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		//Push the root param on the list
		rootParams.push_back(rp);
	}

	D3D12_STATIC_SAMPLER_DESC sd;
	sd.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sd.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sd.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sd.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sd.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sd.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	sd.MaxAnisotropy = 0;
	sd.MaxLOD = D3D12_FLOAT32_MAX;
	sd.MinLOD = 0.f;
	sd.MipLODBias = 0;
	sd.RegisterSpace = 0;
	sd.ShaderRegister = 0;
	sd.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	rsDesc.NumParameters = rootParams.size();
	rsDesc.pParameters = rootParams.data();
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pStaticSamplers = &sd;

	Microsoft::WRL::ComPtr<ID3DBlob> sBlob;

	HRESULT hr = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		sBlob.GetAddressOf(),
		nullptr);

	DX::ThrowIfFailed(gDevice->CreateRootSignature(
		0,
		sBlob->GetBufferPointer(),
		sBlob->GetBufferSize(),
		IID_PPV_ARGS(gRootSignature.GetAddressOf())));
}

void Rootsignature::bindRootSignature()
{
	commandList->SetGraphicsRootSignature(getRootSignature());
}

ID3D12RootSignature * Rootsignature::getRootSignature()
{
	return gRootSignature.Get();
}

void Rootsignature::setRootTableData()
{
	if(descriptorHeaps.size() > 0)
		commandList->SetDescriptorHeaps(descriptorHeaps.size(), descriptorHeaps.data());
	for (int i = 0; i < slots.size(); i++)
		commandList->SetGraphicsRootDescriptorTable(slots[i], descriptorHeaps[i]->GetGPUDescriptorHandleForHeapStart());
	//Reset table lists
	descriptorHeaps.clear();
	slots.clear();
}
