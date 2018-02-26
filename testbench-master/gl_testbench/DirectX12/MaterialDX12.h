#pragma once
#include "../Material.h"
#include <vector>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <string>
#include <wrl\client.h>
#include "IA.h"
#include "..\Renderer.h"
#include "ConstantBufferDX12.h"
#include "RootSignature.h"

class DirectX12Renderer;


class MaterialDX12 :
	public Material
{
	friend DirectX12Renderer;

public:
	MaterialDX12(const std::string& name, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList, Microsoft::WRL::ComPtr<ID3D12Device>& device, std::string shaderPath, Rootsignature* rs );
	~MaterialDX12();
	static int indexCounter;
	int ID;
	void setShader(const std::string& shaderFileName, ShaderType type);
	void removeShader(ShaderType type);
	int compileMaterial(std::string& errString);
	int enable();
	int enable(Renderer *renderer);
	void disable();

	void setDiffuse(Color c);
	void fillPSODesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC &psoDesc);
	// location identifies the constant buffer in a unique way
	void updateConstantBuffer(const void* data, size_t size, unsigned int location);
	// slower version using a string
	void addConstantBuffer(std::string name, unsigned int location);
	std::map<unsigned int, ConstantBufferDX12*> constantBuffers;
private:
	int compileShader(ShaderType type);
	std::string shaderPath;
	//std::map <ShaderType, D3D12_SHADER_BYTECODE> shaders;
	std::map <ShaderType, Microsoft::WRL::ComPtr<ID3DBlob>> shaders;
	std::string name;
	D3D_SHADER_MACRO getMacro(LPCSTR name, LPCSTR value);
	bool makeShaderDefineFile(std::string defines, std::string filename);
	std::vector<D3D_SHADER_MACRO> calculateMacros(std::string macros, std::vector<std::string> &stringValues);
	std::vector<std::string> expandShaderText(std::string& shaderText, ShaderType type);
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;
	Rootsignature* pRS;

	void bindConstBuffers();

};

