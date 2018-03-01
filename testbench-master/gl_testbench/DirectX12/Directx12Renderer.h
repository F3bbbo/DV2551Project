#pragma once
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_5.h> //Only used for initialization of the device and swap chain.
#include <memory>
#include "../Renderer.h"
#include "../Utilityfunctions.h"
#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "d3dcompiler.lib")
#include "RootSignature.h"
#include "PipelineStateDX12.h"
#include "CameraDX12.h"


#include <SDL.h>
#include <GL/glew.h>


#include <iostream>
#include <wrl\client.h>
struct ClAcFc //Commandlist Allocator Fence struct
{
	Microsoft::WRL::ComPtr<ID3D12Fence> Fence;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#include "Defines.h"
class DirectX12Renderer : public Renderer
{
public:
	DirectX12Renderer();
	~DirectX12Renderer();
	std::shared_ptr<Material> makeMaterial(const std::string& name);
	std::shared_ptr<Material> makeMaterial(const std::string& name, int Thread);
	std::shared_ptr<Mesh> makeMesh();
	std::shared_ptr<VertexBuffer> makeVertexBuffer(size_t size, VertexBuffer::DATA_USAGE usage);
	std::shared_ptr<VertexBuffer> makeVertexBuffer(size_t size, VertexBuffer::DATA_USAGE usage,int ThreadID);
	std::shared_ptr<Texture2D> makeTexture2D();
	std::shared_ptr<Texture2D> makeTexture2D(int ThreadID);
	std::shared_ptr<Sampler2D> makeSampler2D();
	std::shared_ptr<RenderState> makeRenderState();
	std::string getShaderPath();
	std::string getShaderExtension();
	std::shared_ptr<ConstantBuffer> makeConstantBuffer(std::string NAME, unsigned int location, int ThreadID);
	std::shared_ptr<ConstantBuffer> makeConstantBuffer(std::string NAME, unsigned int location);
	std::shared_ptr<Technique> makeTechnique(std::shared_ptr<Material>, std::shared_ptr<RenderState>);
	CameraDX12* camera;
	std::map<int, ClAcFc> Thread;
	void CreateClAcFcThread();
	 HWND InitWindow(HINSTANCE hInstance,int width, int height);



	 int initialize(unsigned int width = 800, unsigned int height = 600);
	 void setWinTitle(const char* title);
	 void present();
	 void present(int ThreadID);
	 int shutdown();
	 int shutdown(int ThreadID);
	void setClearColor(float, float, float, float);
	void clearBuffer(unsigned int);
	void clearBuffer(unsigned int opts, int ThreadID);
	// can be partially overriden by a specific Technique.
	void setRenderState(RenderState* ps);
	// submit work (to render) to the renderer.
	void submit(Mesh* mesh);
	virtual void frame();
	void frame(int ThreadID);
	void waitForGPU();
	void waitForGPU(int ThreadID);
	void signalGPU(Microsoft::WRL::ComPtr<ID3D12Fence> Fence, const UINT64 value);
	void signalGPU(Microsoft::WRL::ComPtr<ID3D12Fence> Fence, const UINT64 value, int ThreadID);
	void waitForGPU(Microsoft::WRL::ComPtr<ID3D12Fence> Fence, const UINT64 value, float waittime);
	void waitForGPU(Microsoft::WRL::ComPtr<ID3D12Fence> Fence, const UINT64 value, float waittime,int ThreadID);
	Microsoft::WRL::ComPtr<ID3D12Device> getDevice();
	void setMaterialState(MaterialDX12 *material);
	void updateCamera();

private:
	//Window vars
	MSG msg;
	HWND wndHandle;			//1. Create Window
	int width, height;
	D3D12_VIEWPORT viewPort;
	D3D12_RECT scissorRect;
	//Fence vars
	HANDLE eventHandle = nullptr;
	UINT64 fenceValue = 0;
//	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	//PipelineState
	PipelineStateDX12 pipelineState;
	int frameindex;
	void createDevice();
	void createFenceAndDescriptorSizes();
	void createCommandObject();
	void createSwapChain(HWND& wndHandle);
	void createRTV();
	void createViewPortScissor();
	void createDescriptorHeaps();
	void createDepthStencil();
	ID3D12DescriptorHeap*	gDescriptorHeap[1];
	Rootsignature Root;
	//Device
	Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;
	//Command list/queue
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
//	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
//	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	void executeCommandList();
	void executeCommandList(int ThreadID);
	//Swap Chain
	D3D12_CPU_DESCRIPTOR_HANDLE currDescHandle;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RTVHeap;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainBuffers[SWAP_BUFFER_COUNT];
	int currBackBuffer = 0;
	unsigned int RTVDescriptorSize, DSVDescriptorSize, CBVSRVDescriptorSize;
	D3D12_CPU_DESCRIPTOR_HANDLE getCurrBackBuffView();
	//Depth stencil
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DSVHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE getDepthView();
	//Draw list
	int perMat = 1;
	std::vector<Mesh*> drawList;
	std::unordered_map<Technique*, std::vector<Mesh*>> drawList2;

	float clearColor[4] = { 0,0,0,0 };
};