#include <string>
#include <SDL_keyboard.h>
#include <SDL_events.h>
#include <SDL_timer.h>
#pragma comment(lib,"SDL2.lib")
#include <type_traits> 
#include <assert.h>

#include "Renderer.h"
#include "DirectX12/Directx12Renderer.h"
#include "Mesh.h"
#include "Texture2D.h"
#include <math.h>
#include <memory>
#include <queue>

//#include "DirectX12/MeshDX12.h"
#include "MeshReader.h"
#include "Grid.h"
#include "IA.h"

//Thread include
#include "ThreadFunctions.h"
#include <future>

//testing includes
#include <stdlib.h>     /* srand, rand */
#include <time.h>
#include <SimpleMath.h>

#include <process.h>
#include <mutex>

#include <time.h>
using namespace std;
DirectX12Renderer* renderer;
Grid* grid;
// flat scene at the application level...we don't care about this here.
// do what ever you want in your renderer backend.
// all these objects are loosely coupled, creation and destruction is responsibility
// of the testbench, not of the container objects
struct int2
{
	int2(int x, int y) { this->x = x; this->y = y; };
	int2() { x = 0; y = 0; };
	int x, y;

	bool operator==(const int2& o)
	{
		return x == o.x && y == o.y;
	}
};

struct cellRender
{
	bool isReady;
	int thread;
	int2 cell;
	vector<shared_ptr<Mesh>>* objects;

	cellRender() { isReady = false; cell = int2(-1, -1); objects = nullptr; thread = -1; };
};

vector<cellRender*> objectsToRender;


vector<std::shared_ptr<Mesh>> scene;
vector<std::shared_ptr<Material>> materials;
vector<std::shared_ptr<Technique>> techniques;
vector<std::shared_ptr<Texture2D>> textures;
vector<std::shared_ptr<Sampler2D>> samplers;

MeshReader *meshReader = nullptr;

std::shared_ptr<VertexBuffer> pos;
std::shared_ptr<VertexBuffer> nor;
std::shared_ptr<VertexBuffer> uvs;

// forward decls
void updateScene();
void renderScene();

void updateGridList();
char gTitleBuff[256];
double gLastDelta = 0.0;

float deltatimeGlobale=0.0000011;

std::shared_ptr<Material> triangleMaterial;
std::shared_ptr<RenderState> triangleRS;
std::shared_ptr<Technique> triangleT;

void LaunchThreads();
void CheckThreadLoading();
bool idleThreads[NUMBER_OF_LOADING_THREADS];
HANDLE threads[NUMBER_OF_LOADING_THREADS];
vector<int2> activeCells;

const int2 gridStart = { -5, -5 };

void updateDelta()
{
	#define WINDOW_SIZE 10
	static Uint64 start = 0;
	static Uint64 last = 0;
	static double avg[WINDOW_SIZE] = { 0.0 };
	static double lastSum = 10.0;
	static int loop = 0;

	last = start;
	start = SDL_GetPerformanceCounter();
	double deltaTime = (double)((start - last) * 1000.0 / SDL_GetPerformanceFrequency());
	deltatimeGlobale = deltaTime;
	// moving average window of WINDOWS_SIZE
	lastSum -= avg[loop];
	lastSum += deltaTime;
	avg[loop] = deltaTime;
	loop = (loop + 1) % WINDOW_SIZE;
	gLastDelta = (lastSum / WINDOW_SIZE);
};

// TOTAL_TRIS pretty much decides how many drawcalls in a brute force approach.
constexpr int TOTAL_TRIS = 100.0f;
// this has to do with how the triangles are spread in the screen, not important.
constexpr int TOTAL_PLACES = 2 * TOTAL_TRIS;
float xt[TOTAL_PLACES], yt[TOTAL_PLACES];

// lissajous points
typedef union { 
	struct { float x, y, z, w; };
	struct { float r, g, b, a; };
} float4;
/*
typedef union { 
	struct { float x, y; };
	struct { float u, v; };
} float2;
*/

void run() {

	SDL_Event windowEvent;
	while (true)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_QUIT) break;
			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) break;
		}
		updateGridList();
		LaunchThreads();
		CheckThreadLoading();
		/*for (int i = 0; i < activeCells.size(); i++)
			cout << activeCells[i].x << " " << activeCells[i].y << ", ";
		cout << endl;*/
		updateScene();
		renderScene();
	}
}

/*
 update positions of triangles in the screen changing a translation only
*/


void updateScene()
{
	
	renderer->updateCamera(deltatimeGlobale);
	// cout << scene.size() << endl;
	// Check if new grids needs to be loaded and add them to a grid list.
	// Check if the list associated to a thread previously launched had finished by checking a fence, if the fence has been reached, add integers to the "idleThreads" queue indicating that a new thread can be launched with that command list.
	// Launch threads. Each thread is responisble for loading one grid cell, launch as many threads as available
		// A thread is given a vector of objects to be loaded. For each mesh the thread should:
			// Load the data from the file.
			// Create position, index , UVs and Normal vertex buffers with the appropriate resource state.
			// Each file to be loaded has a texture associated with it which needs to be loaded.
			// Upload the vertex data and texture to the GPU
			// When the mesh is uploaded, change the command list pointer that he mesh hold to the main threads command list.
			



	/*
	    For each mesh in scene list, update their position 
	*/
	//{
	//	static long long shift = 0;
	//	const int size = scene.size();
	//	for (int i = 0; i < size; i++)
	//	{
	//		const float4 trans { 
	//			xt[(int)(float)(i + shift) % (TOTAL_PLACES)], 
	//			yt[(int)(float)(i + shift) % (TOTAL_PLACES)], 
	//			i * (-1.0 / TOTAL_PLACES),
	//			0.0
	//		};
	//		scene[i]->txBuffer->setData(&trans, sizeof(trans), scene[i]->technique->getMaterial(), TRANSLATION);
	//	}
	//	// just to make them move...
	//	shift+=max(TOTAL_TRIS / 1000.0,TOTAL_TRIS / 100.0);
	//}
	return;
};


void renderScene()
{
	renderer->clearBuffer(CLEAR_BUFFER_FLAGS::COLOR | CLEAR_BUFFER_FLAGS::DEPTH);
	for (auto m : scene)
	{
		renderer->submit(m.get());
	}
	renderer->frame();
	renderer->present();
	updateDelta();
	sprintf(gTitleBuff, "DirectX12 - Dynamic scene loader test - %3.0lf", gLastDelta);
	renderer->setWinTitle(gTitleBuff);
}

int initialiseTestbench()
{
	//MeshReader TEST
	MeshReader mr(renderer);
	mr.LoadFromFile("Models/NewLowPolyTree.fbx","Models/PolyTreeTexture.png", scene);
	// triangle geometry:
	float4 triPos[3] = { { 0.0f,  1.0, 0.0f, 1.0f },{ 1.0, -1.0, 0.0f, 1.0f },{ -1.0, -1.0, 0.0f, 1.0f } };
	float4 triNor[3] = { { 0.0f,  0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f, 0.0f } };
	float2 triUV[3] =  { { 0.5f,  -0.99f },{ 1.49f, 1.1f },{ -0.51, 1.1f } };


	//Make material
	std::string shaderPath = renderer->getShaderPath();
	std::string shaderExtension = renderer->getShaderExtension();

	std::shared_ptr<Material> m = renderer->makeMaterial(shaderPath + "VertexShader" + shaderExtension);
	
	m->setShader(shaderPath + "VertexShader" + shaderExtension, Material::ShaderType::VS);
	m->setShader(shaderPath + "FragmentShader" + shaderExtension, Material::ShaderType::PS);

	std::string err;
	m->compileMaterial(err);

	materials.push_back(m);

	std::shared_ptr<RenderState> rs = renderer->makeRenderState();

	std::shared_ptr<Technique> t = renderer->makeTechnique(m, rs);

	techniques.push_back(t);

	for (unsigned int i = 0; i < scene.size(); i++)
	{
		scene[i]->technique = t;
	}

	//Allocate vertex buffers
	//pos = renderer->makeVertexBuffer(sizeof(triPos), VertexBuffer::DATA_USAGE::DONTCARE);
	//nor = renderer->makeVertexBuffer(sizeof(triNor), VertexBuffer::DATA_USAGE::DONTCARE);
	//uvs = renderer->makeVertexBuffer(sizeof(triUV), VertexBuffer::DATA_USAGE::DONTCARE);

	//Create mesh
	//std::shared_ptr<Mesh> mesh = renderer->makeMesh();
	//pos->setData(triPos, sizeof(triPos), 0);
	//mesh->addIAVertexBufferBinding(pos, 0, ARRAYSIZE(triPos), sizeof(float4), POS);
	//
	//nor->setData(triNor, sizeof(triNor), 0);
	//mesh->addIAVertexBufferBinding(nor, 0, ARRAYSIZE(triNor), sizeof(float4), NORM);

	//uvs->setData(triUV, sizeof(triUV), 0);
	//mesh->addIAVertexBufferBinding(uvs, 0, ARRAYSIZE(triUV), sizeof(float2), UVCOORD);

	//mesh->technique = t;

	//scene.push_back(mesh);
	return 0;
}

void shutdown() {
	// shutdown.
	// delete dynamic objects
	//for (auto m : materials)
	//{
	//	delete(m);
	//}
	//for (auto t : techniques)
	//{
	//	delete(t);
	//}
	//for (auto m : scene)
	//{
	//	delete(m);
	//};
	//assert(pos->refCount() == 0);
	//delete pos;
	//assert(nor->refCount() == 0);
	//delete nor;
	//assert(uvs->refCount() == 0);
	//delete uvs;
	//
	//for (auto s : samplers)
	//{
	//	delete s;
	//}

	//for (auto t : textures)
	//{
	//	delete t;
	//}
	delete meshReader;
	renderer->shutdown();
};
void fillCell(int x, int y, int amount)
{
	srand(time(NULL));

	float randNumb;
	for (int i = 0; i < amount; i++)
	{
		randNumb = (float)(rand() % 1000) / 1000.f;
		float pos[3] = { x * cellWidth + randNumb, 1, y * cellWidth + randNumb };
		float scale[3] = { 1, 1, 1 };
		float rot[3] = { 0, 0, 0 };
		Object* object = new Object(pos, scale, rot, "Models/NewLowPolyTree.fbx","Models/PolyTreeTexture.png");
		grid->addMesh(x, y, object);
	}

}


threadinfo* createThread(HANDLE &threadHandle, std::vector<Object*> &objectList, std::vector<std::shared_ptr<Mesh>>* meshes, int threadKey)
{
	//    std::cout << (*grid)[0][0]->objectList.size(); 
	//    std::cout << (*grid)[0][1]->objectList.size(); 
	//    std::cout << (*grid)[0][2]->objectList.size(); 
//	std::vector<Object*> data1 = (*grid)[0][0]->objectList;
//	std::vector<Object*> data2 = (*grid)[0][1]->objectList;
//	std::vector<Object*> data3 = (*grid)[0][2]->objectList;
	//    std::cout << (*grid)[0][0]->objectList[0]->position.x << std::endl; 
	threadinfo* data1 = new threadinfo();
	data1->size = objectList.size();
	data1->data = objectList.data();
	data1->key = threadKey;
	data1->meshes = meshes;
	data1->reader = meshReader;
	data1->renderer = renderer;
	data1->technique = &triangleT;


	threadHandle = (HANDLE)_beginthreadex(0, 0, &threadfunctionloadingdata, data1, 0, 0);
	//WaitForSingleObject(threadHandle, INFINITE);
	//CloseHandle(threadHandle);
	return data1;
}
void fillGrid()
{
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			fillCell(x, y, 2);
		}
		cout << x << endl;
	}
}

void createGlobalData()
{
	std::string shaderPath = renderer->getShaderPath();
	std::string shaderExtension = renderer->getShaderExtension();

	triangleMaterial = renderer->makeMaterial(shaderPath + "VertexShader" + shaderExtension);

	triangleMaterial->setShader(shaderPath + "VertexShader" + shaderExtension, Material::ShaderType::VS);
	triangleMaterial->setShader(shaderPath + "FragmentShader" + shaderExtension, Material::ShaderType::PS);

	std::string err;
	triangleMaterial->compileMaterial(err);

	materials.push_back(triangleMaterial);

	triangleRS = renderer->makeRenderState();
	triangleT = renderer->makeTechnique(triangleMaterial, triangleRS);

	techniques.push_back(triangleT);
}

void updateGridList()
{
	Vector2 camPos = { renderer->camera->getPosition().x, renderer->camera->getPosition().z };
	int xStartDist = min(max(0, int(((int)camPos.x - LOADINGTHRESHOLD) / (float)cellWidth)), WWidth);
	int yStartDist = min(max(0, int(((int)camPos.y - LOADINGTHRESHOLD) / (float)cellHeight)), HHeight);
	int xEndDist = min(max(0, int(((int)camPos.x + LOADINGTHRESHOLD) / (float)cellWidth)), WWidth);
	int yEndDist = min(max(0, int(((int)camPos.y + LOADINGTHRESHOLD) / (float)cellHeight)), HHeight);

	//Check if cells needs to be loaded
	for (int x = xStartDist; x <= xEndDist; x++)
	{
		for (int y = yStartDist; y <= yEndDist; y++)
		{
			if ((*grid)[x][y]->status == NOT_LOADED)
			{
				(*grid)[x][y]->status = PENDING_LOAD;
				activeCells.push_back(int2(x, y));
			}
		}
	}

	//Remove
	for (int i = 0; i < activeCells.size(); i++)
	{
		int x = activeCells[i].x;
		int y = activeCells[i].y;
		if (x < xStartDist || x > xEndDist || y < yStartDist || y > yEndDist)
		{
			if ((*grid)[x][y]->status == PENDING_LOAD)
			{
				(*grid)[x][y]->status = NOT_LOADED;
				activeCells.erase(activeCells.begin() + i);
			}
			if ((*grid)[x][y]->status == LOADED)
			{
				(*grid)[x][y]->status = NOT_LOADED;
				for (int j = 0; j < objectsToRender.size(); j++)
				{
					if (objectsToRender[i]->cell == int2(x, y))
					{
						objectsToRender.erase(objectsToRender.begin() + i);
						break;
					}
				}
			}
		}
	}
}
void LaunchThreads()
{
	for (int tID = 0; tID < NUMBER_OF_LOADING_THREADS; tID++)
	{
		if (idleThreads[tID])
		{
			int cellIndex = -1;

			// Find a cell that a thread should start to load
			for (int j = 0; j < activeCells.size(); j++)
			{
				if ((*grid)[activeCells[j].x][activeCells[j].y]->status == PENDING_LOAD)
				{
					(*grid)[activeCells[j].x][activeCells[j].y]->status == LOADING;
					cellIndex = j;
					break;
				}
			}
			if (cellIndex == -1) // return if no grid cells needs to be loaded
				return;

			cellRender* cell = new cellRender();
			cell->cell = int2(activeCells[cellIndex].x, activeCells[cellIndex].y);
			cell->objects = new std::vector<std::shared_ptr<Mesh>>();
			cell->thread = tID;
			objectsToRender.push_back(cell);

			createThread(threads[tID], (*grid)[activeCells[cellIndex].x][activeCells[cellIndex].y]->objectList, cell->objects, tID);
			idleThreads[tID] = false;
		}
	}
}

void CheckThreadLoading()
{
	for (int i = 0; i < NUMBER_OF_LOADING_THREADS; i++)
	{
		// check if a thread is working, if it is, check if the thread has finished. Not sure if that check is working or not...
		if (!(idleThreads[i]) && WAIT_OBJECT_0 == WaitForSingleObject(threads[i], 0))
		{
			int index = -1;
			// Since the thread has finished it has loaded all of the meshes in one grid cell, however, we need to find which cell the thread is responsible for
			for (int j = 0; j < objectsToRender.size(); j++)
			{
				if (objectsToRender[j]->thread == i && objectsToRender[j]->isReady == false)
					index = j;
			}

			// mikaels funtion, set command list such that we can execute it and change resource state.
			for (unsigned int k = 0; k < (*objectsToRender[index]->objects).size(); k++)
			{
				renderer->setDirectList((*objectsToRender[index]->objects)[k].get(), MAIN_THREAD);
			}
			renderer->executeDirectCommandList(i);
			renderer->signalDirect(1, i);

			// might as well wait for it when we're testing.
			renderer->waitForDirect(1, INFINITY, i);
			for (int k = 0; k < (*objectsToRender[index]->objects).size(); k++)
			{
				scene.push_back((*objectsToRender[index]->objects)[k]);
			}
			
			// tell the renderer that the list is ready to draw
			(*grid)[objectsToRender[index]->cell.x][objectsToRender[index]->cell.y]->status = LOADED;
			objectsToRender[index]->isReady = true;
			idleThreads[i] = true;
		}
	}
}
#undef main
int main(int argc, char *argv[])
{
	renderer = static_cast<DirectX12Renderer*>(Renderer::makeRenderer(Renderer::BACKEND::DX12));
	renderer->initialize(800, 600);
	renderer->setWinTitle("DirectX12 - Dynamic scene loader test");
	renderer->setClearColor(0.0, 0.1, 0.1, 1.0);
	meshReader = new MeshReader(renderer);
	createGlobalData();
	grid = new Grid();
	grid->createGrid(WWidth, HHeight);
	fillGrid();

	for (int i = 0; i < NUMBER_OF_LOADING_THREADS; i++)
		idleThreads[i] = true;

	//(*grid)[0].size();
	//Vector3 pos = (*grid)[0][0]->objectList[0]->position;
	//initialiseTestbench();
	run();
	shutdown();
	return 0;
};
