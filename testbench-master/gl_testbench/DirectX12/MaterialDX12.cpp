#include "MaterialDX12.h"
#include <istream>
#include <fstream>
#include <vector>
#include < set>
#include <assert.h>
#include "d3dUtil.h"
#include "Directx12Renderer.h"
#include "../Utilityfunctions.h"

int MaterialDX12::indexCounter;

MaterialDX12::MaterialDX12(const std::string & _name, 
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList ,
	Microsoft::WRL::ComPtr<ID3D12Device> &device, 
	std::string shaderPath, 
	Rootsignature* rs)
{
	isValid = true;
	name = _name;
	ID = indexCounter++;
	pRS = rs;
	this->commandList = cmdList;
	this->device = device;
	this->shaderPath = shaderPath;
}

MaterialDX12::~MaterialDX12()
{
}

void MaterialDX12::setShader(const std::string & shaderFileName, ShaderType type)
{
	if (shaderFileNames.find(type) != shaderFileNames.end())
	{
		removeShader(type);
	}
	shaderFileNames[type] = shaderFileName;
}

void MaterialDX12::removeShader(ShaderType type)
{
	auto shader = shaders.find(type);
	if (shader != shaders.end()) // If the iterator finds a element wuth the type
	{
		shaders.erase(shader);
	}
	return;
}

int MaterialDX12::compileMaterial(std::string & errString)
{
	removeShader(ShaderType::VS);
	removeShader(ShaderType::PS);

	// compiles the shaders
	compileShader(ShaderType::VS);
	compileShader(ShaderType::PS);

	return 0;
}

int MaterialDX12::enable()
{
	return 0;
}

int MaterialDX12::enable(Renderer * renderer)
{
	DirectX12Renderer* pDX12Renderer = dynamic_cast<DirectX12Renderer*>(renderer);
	if (pDX12Renderer != nullptr)
	{
		pDX12Renderer->setMaterialState(this);
		bindConstBuffers();
	}
	else
	{
		printf("Error: Not a DirectX12Renderer in MaterialDX12::enable().");
	}
	return 0;
}

void MaterialDX12::disable()
{
}

void MaterialDX12::setDiffuse(Color c)
{

}

void MaterialDX12::fillPSODesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC & psoDesc)
{
	psoDesc.InputLayout = { inputDescriptions.data(), (UINT) inputDescriptions.size() };
	psoDesc.VS = { shaders[ShaderType::VS]->GetBufferPointer(), shaders[ShaderType::VS]->GetBufferSize() };
	psoDesc.PS = { shaders[ShaderType::PS]->GetBufferPointer(), shaders[ShaderType::PS]->GetBufferSize() };
}

void MaterialDX12::updateConstantBuffer(const void * data, size_t size, unsigned int location)
{
	constantBuffers[location]->setData(data, size, this, location);
}

void MaterialDX12::addConstantBuffer(std::string name, unsigned int location)
{
	constantBuffers[location] = new ConstantBufferDX12(device.Get(),name, location, commandList.Get(), pRS);
}

int MaterialDX12::compileShader(ShaderType type)
{
	std::string defines = "";
	std::string fileName;
	std::map<ShaderType, std::string>::iterator itShaderFileNames;

	itShaderFileNames = shaderFileNames.find(type);
	if (itShaderFileNames != shaderFileNames.end())
	{
		for (auto define : shaderDefines[type])
			defines += define;

		Microsoft::WRL::ComPtr<ID3DBlob> error;
		Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	
		std::string version;
		if (type == ShaderType::VS)
		{
			version = "vs_5_0";
			createInputDesc(defines);
		}
		else if (type == ShaderType::PS)
		{
			version = "ps_5_0";
		}
		//Define macros code
		std::vector<std::string> macroValueStrings;
		macroValueStrings.reserve(10);
		const std::vector<D3D_SHADER_MACRO> macros(calculateMacros(defines, macroValueStrings));
		shaders[type] = d3dUtil::CompileShader(convert(shaderFileNames[type]), macros.data(), "main", version.c_str());
		defines = "";
	}
	return 0;
}

void MaterialDX12::createInputDesc(std::string macros)
{
	inputDescriptions.clear();

	if (macros.find("POSITION") != std::string::npos)
	{
		inputDescriptions.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, POSITION, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		//offset += sizeof(float) * 4;
	}

	if (macros.find("NORMAL") != std::string::npos)
	{
		inputDescriptions.push_back({ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, NORMAL, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		//offset += sizeof(float) * 4;
	}

	if (macros.find("TEXTCOORD") != std::string::npos)
	{
		inputDescriptions.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, TEXTCOORD, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
		//offset += sizeof(float) * 4;
	}

	return;
}

D3D_SHADER_MACRO MaterialDX12::getMacro(LPCSTR name, LPCSTR value)
{
	D3D_SHADER_MACRO macro;
	macro.Definition = value;
	macro.Name = name;
	return macro;
}

bool MaterialDX12::makeShaderDefineFile(std::string defines, std::string filename)
{
	//Open file
	std::ofstream file;
	file.open(shaderPath + filename, std::ofstream::out);

	if (defines.find("POSITION") != std::string::npos)
	{
		file << "#define POSITIONDEF " << std::to_string(POSITION) << "\n";
	}
	if (defines.find("NORMAL") != std::string::npos)
	{
		file << "#define NORMALDEF " << std::to_string(NORMAL) << "\n";
	}
	if (defines.find("TEXTCOORD") != std::string::npos)
	{
		file << "#define TEXTCOORDDEF " << std::to_string(TEXTCOORD) << "\n";
	}
	if (defines.find("INDEXBUFF") != std::string::npos)
	{
		file << "#define INDEXBUFFDEF " << std::to_string(INDEXBUFF) << "\n";
	}
	if (defines.find("TRANSLATION") != std::string::npos)
	{
		file << "#define TRANSLATIONDEF b" << std::to_string(TRANSLATION) << "\n";
	}
	if (defines.find("TRANSLATION_NAME") != std::string::npos)
	{
		file << "#define TRANSLATION_NAMEDEF " << TRANSLATION_NAME << "\n";
	}
	if (defines.find("DIFFUSE_TINT") != std::string::npos)
	{
		file << "#define DIFFUSE_TINTDEF b" << std::to_string(DIFFUSE_TINT) << "\n";
	}
	if (defines.find("DIFFUSE_TINT_NAME") != std::string::npos)
	{
		file << "#define DIFFUSE_TINT_NAMEDEF " << DIFFUSE_TINT_NAME << "\n";
	}
	if (defines.find("DIFFUSE_SLOT") != std::string::npos)
	{
		file << "#define DIFFUSE_SLOTDEF t" << std::to_string(DIFFUSE_SLOT) << "\n";
	}
	//Close file
	file.close();
	return false;
}

std::vector<D3D_SHADER_MACRO> MaterialDX12::calculateMacros(std::string macros, std::vector<std::string> &stringValues)
{
	std::vector<D3D_SHADER_MACRO> shaderMacros;

	if (macros.find("POSITION") != std::string::npos)
	{
		stringValues.push_back(std::to_string(POSITION));
		shaderMacros.push_back(getMacro("POSITIONDEF", stringValues.back().data()));
	}
	if (macros.find("NORMAL") != std::string::npos)
	{
		stringValues.push_back(std::to_string(NORMAL));
		shaderMacros.push_back(getMacro("NORMALDEF", stringValues.back().data()));
	}

	if (macros.find("TEXTCOORD") != std::string::npos)
	{
		stringValues.push_back(std::to_string(TEXTCOORD));
		shaderMacros.push_back(getMacro("TEXTCOORDDEF", stringValues.back().data()));
	}
	if (macros.find("INDEXBUFF") != std::string::npos)
	{
		stringValues.push_back(std::to_string(INDEXBUFF));
		shaderMacros.push_back(getMacro("INDEXBUFFDEF", stringValues.back().data()));
	}
	if (macros.find("TRANSLATION") != std::string::npos)
	{
		stringValues.push_back(std::string("b") + std::to_string(TRANSLATION));
		shaderMacros.push_back(getMacro("TRANSLATIONDEF", stringValues.back().data()));
	}

	if (macros.find("TRANSLATION_NAME") != std::string::npos)
		shaderMacros.push_back(getMacro("TRANSLATION_NAMEDEF", TRANSLATION_NAME));

	if (macros.find("DIFFUSE_TINT") != std::string::npos)
	{
		stringValues.push_back(std::string("b") + std::to_string(DIFFUSE_TINT));
		shaderMacros.push_back(getMacro("DIFFUSE_TINTDEF", stringValues.back().data()));
	}

	if (macros.find("DIFFUSE_TINT_NAME") != std::string::npos)
		shaderMacros.push_back(getMacro("DIFFUSE_TINT_NAMEDEF", DIFFUSE_TINT_NAME));

	if (macros.find("DIFFUSE_SLOT") != std::string::npos)
	{
		stringValues.push_back(std::string("t") + std::to_string(DIFFUSE_SLOT));
		shaderMacros.push_back(getMacro("DIFFUSE_SLOTDEF", stringValues.back().data()));
	}

	shaderMacros.push_back(getMacro(NULL, NULL));
	return shaderMacros;
}

std::vector<std::string> MaterialDX12::expandShaderText(std::string & shaderText, ShaderType type)
{
	return std::vector<std::string>();
}

void MaterialDX12::bindConstBuffers()
{
	for (auto const& cBuff : constantBuffers)
	{
		cBuff.second->bind(this);
	}
}
