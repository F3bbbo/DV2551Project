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
#include "Defines.h"

#include <mutex>

#include <SDL.h>
#include <GL/glew.h>


#include <iostream>
#include <wrl\client.h>
struct ClAcFc //Commandlist Allocator Fence struct
{
	Microsoft::WRL::ComPtr<ID3D12Fence> fenceCopy;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> copyCommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> copyCommandList;
	Microsoft::WRL::ComPtr<ID3D12Fence> fenceDirect;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> directCommandList;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> directCommandAllocator;
};
typedef union {
	struct { float x, y; };
	struct { float u, v; };
} float2;

typedef union {
	struct { float x, y,z; };
	struct { float u, v,w; };
} float3;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
class DirectX12Renderer : public Renderer
{
public:
	DirectX12Renderer();
	~DirectX12Renderer();
	std::shared_ptr<Material> makeMaterial(const std::string& name);
	std::shared_ptr<Material> makeMaterial(const std::string& name, int Thread);
	std::shared_ptr<Mesh> makeMesh();
	std::shared_ptr<Mesh> makeMesh(unsigned int key);
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
	void setCopyList(Mesh *mesh, int ThreadID);
	void setDirectList(Mesh* mesh, int ThreadID);
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
	void signalDirectThread(int Value, int ThreadID);
	void signalDirectDraw(int Value, int ThreadID);
	bool waitForDirect(int Value, float waitTime);
	bool waitForDirect(int Value, float waitTime, int ThreadID);
	void signalCopy(int Value, int ThreadID);
	bool waitForCopy(int Value, float waitTime);
	bool waitForCopy(int Value, float waitTime, int ThreadID);
	Microsoft::WRL::ComPtr<ID3D12Device> getDevice();
	void setMaterialState(MaterialDX12 *material);
	void updateCamera(float delta);
	void createwalkingpath();
	void executeCopyCommandList(int ThreadID);
	void executeDirectDrawCommandList(int threadID);
	void resetCopyCommandList(int threadID);
	void resetDirectCommandList(int threadID);

private:
	int m;
	std::vector<float3> walkingpath;
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
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandDrawQueue = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueueCopy = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueueThreadDirect = nullptr;
//	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
//	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	void executeCommandList(ID3D12CommandQueue* cmdQueue);
	void executeCommandList(ID3D12CommandQueue* cmdQueue, int ThreadID);
	void signalGPU(ID3D12CommandQueue* cmdQueue, Microsoft::WRL::ComPtr<ID3D12Fence> Fence, const UINT64 value);
	void signalGPU(ID3D12CommandQueue* cmdQueue, Microsoft::WRL::ComPtr<ID3D12Fence> Fence, const UINT64 value, int ThreadID);
	bool waitForGPU(Microsoft::WRL::ComPtr<ID3D12Fence> Fence, const UINT64 value, float waittime);
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

	std::mutex copyLock;
};