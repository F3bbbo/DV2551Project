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
#include <thread>
#include <fstream>

//testing includes
#include <stdlib.h>     /* srand, rand */
#include <time.h>
#include <SimpleMath.h>

#include <process.h>
#include <mutex>

#include <time.h>

#include "D3D12Timer.hpp"
Vector2 oldCamPos;
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
	vector<shared_ptr<Mesh>>* objects;

	cellRender() { isReady = false; objects = nullptr; thread = -1; };
};

map<int, cellRender*> objectsToRender;


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
vector<vector<int2>> activeCells2;
threadinfo threadData[NUMBER_OF_LOADING_THREADS];

const int2 gridStart = { -5, -5 };

thread dataCollector;

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

		activeCells2;
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
	//vector<std::shared_ptr<Mesh>> scene;
	//vector<shared_ptr<Mesh>>* objects;
	renderer->clearBuffer(CLEAR_BUFFER_FLAGS::COLOR | CLEAR_BUFFER_FLAGS::DEPTH);
	//map<int, cellRender*>::iterator it;
	for (auto cellWork : objectsToRender)
	{
		if (cellWork.second->isReady)
		{
			for (auto m : *(cellWork.second->objects))
			{
				renderer->submit(m.get());
			}
		}
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


void createThread(HANDLE &threadHandle, std::vector<Object*> &objectList, std::vector<std::shared_ptr<Mesh>>* meshes, int threadKey, int cellX, int cellY)
{ 
	threadData[threadKey] =  threadinfo();
	threadData[threadKey].cellX = cellX;
	threadData[threadKey].cellY = cellY;
	threadData[threadKey].size = objectList.size();
	threadData[threadKey].data = objectList.data();
	threadData[threadKey].key = threadKey;
	threadData[threadKey].meshes = meshes;
	threadData[threadKey].reader = meshReader;
	threadData[threadKey].renderer = renderer;
	threadData[threadKey].technique = &triangleT;


	threadHandle = (HANDLE)_beginthreadex(0, 0, &threadfunctionloadingdata, &threadData[threadKey], 0, 0);
	//WaitForSingleObject(threadHandle, INFINITE);
	//CloseHandle(threadHandle);
	return;
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



void addActiveCells()
{
	Vector2 camPos = { renderer->camera->getPosition().x, renderer->camera->getPosition().z };
	int xCell = camPos.x / cellWidth;
	int yCell = camPos.y / cellHeight;
	int xCellold = oldCamPos.x / cellWidth;
	int yCellold = oldCamPos.y / cellHeight;
	if (activeCells.size() == 0)
	{


		for (int x = max(0, xCell - LOADINGTHRESHOLD); x < min(xCell + LOADINGTHRESHOLD,WWidth); x++)
		{
			vector<int2> Xlayer;
			activeCells2.push_back(Xlayer);
			for (int y = max(yCell - LOADINGTHRESHOLD,0); y < min(yCell + LOADINGTHRESHOLD, HHeight); y++)
			{
				(*grid)[x][y]->status = PENDING_LOAD;
				activeCells.push_back(int2(x, y));
				activeCells2[x].push_back(int2(x,y));
			}
		}
		
	}
	else
	{
		if (xCell > xCellold)
		{
			//r�rtsig�th�ger
			vector<int2> Xlayer;
			activeCells2.push_back(Xlayer); //add a new xlayer to the right
			for (int y = max(0,yCell - LOADINGTHRESHOLD); y < min(HHeight,yCell + LOADINGTHRESHOLD); y++)
			{
				(*grid)[min(WWidth,xCell + LOADINGTHRESHOLD)][y]->status = PENDING_LOAD;
				activeCells.push_back(int2(min(HHeight, xCell + LOADINGTHRESHOLD), y));
				activeCells2[min(HHeight, xCell + LOADINGTHRESHOLD-1)].push_back(int2(min(HHeight, xCell + LOADINGTHRESHOLD - 1), y));
			}
		}
		else if(xCell < xCellold)
		{
			vector<int2> Xlayer;
			activeCells2.insert(activeCells2.begin()+ max(0, xCell - LOADINGTHRESHOLD - 1), Xlayer);
			for (int y = max(0, yCell - LOADINGTHRESHOLD); y < min(HHeight, yCell + LOADINGTHRESHOLD); y++)
			{
				(*grid)[max(0,xCell - LOADINGTHRESHOLD)][y]->status = PENDING_LOAD;
				activeCells.push_back(int2(max(0, xCell - LOADINGTHRESHOLD), y));
			//	activeCells2.
				activeCells2[max(0, xCell - LOADINGTHRESHOLD - 1)].push_back(int2(max(0, xCell - LOADINGTHRESHOLD), y));
			}
			//r�rtsig�tv�nster
		}
		if (yCell > yCellold)
		{

			for (int x = min(0, xCell - LOADINGTHRESHOLD); x < min(WWidth, xCell + LOADINGTHRESHOLD); x++)
			{
				(*grid)[x][min(WWidth,yCell +LOADINGTHRESHOLD)]->status = PENDING_LOAD;
				activeCells.push_back(int2(x, min(WWidth, yCell + LOADINGTHRESHOLD)));
				activeCells2[x].push_back(int2(x, min(WWidth, yCell + LOADINGTHRESHOLD)));
			}
			//r�rtsiginn�t
		}
		else if(yCell < yCellold)
		{
			for (int x = max(0, xCell - LOADINGTHRESHOLD); x < min(WWidth, xCell + LOADINGTHRESHOLD); x++)
			{
				(*grid)[x][max(0,yCell - LOADINGTHRESHOLD)]->status = PENDING_LOAD;
				activeCells.push_back(int2(x, max(0, yCell - LOADINGTHRESHOLD)));
				activeCells2[x].insert(activeCells2[x].begin()+ max(0, yCell - LOADINGTHRESHOLD - 1),int2(x, max(0, yCell - LOADINGTHRESHOLD)));
			}

			//r�rtsigut�t
		}
	}


}

void removeNonActiveCells()
{
	Vector2 camPos = { renderer->camera->getPosition().x, renderer->camera->getPosition().z };
	int xCell = camPos.x / cellWidth;
	int yCell = camPos.y / cellHeight;
	int xCellold = oldCamPos.x / cellWidth;
	int yCellold = oldCamPos.y / cellHeight;
	if (xCell > xCellold)
	{
		//r�rtsig�th�ger tar bort det som �r till v�nster i y led
		activeCells2.erase(activeCells2.begin());
		for (int i = 0; i < LOADINGTHRESHOLD*2+1; i++)
		{
			(*grid)[max(0, xCell - LOADINGTHRESHOLD)][i]->status = NOT_LOADED;
		}
	}
	else if (xCell < xCellold)
	{
		//r�rtsig�tv�nster tar bort   det till h�ger.
		activeCells2.erase(activeCells2.begin()+xCell+LOADINGTHRESHOLD - 1);
		for (int i = 0; i < LOADINGTHRESHOLD * 2 + 1; i++)
		{
			(*grid)[max(0, xCell - LOADINGTHRESHOLD)][i]->status = NOT_LOADED;
		}
	}
	if (yCell > yCellold)
	{
		//r�rtsiginn�t ta bort under
	
	//	for(int x = )
		
	}
	else if (yCell < yCellold)
	{
		for (int x = max(0, xCell - LOADINGTHRESHOLD); x < min(WWidth, xCell + LOADINGTHRESHOLD); x++)
		{
			(*grid)[x][max(0, yCell - LOADINGTHRESHOLD)]->status = NOT_LOADED;
			activeCells2[x].insert(activeCells2[x].begin() + max(0, yCell - LOADINGTHRESHOLD - 1), int2(x, max(0, yCell - LOADINGTHRESHOLD)));
		}

		//r�rtsigut�t
	}

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

	//addActiveCells();

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
			// The code seems to be working, but since we still need to remove the meshes from the scene vector the following code is outcommented
			if ((*grid)[x][y]->status == LOADED)
			{
				(*grid)[x][y]->status = NOT_LOADED;
				//objectsToRender[y * WWidth + x]->isReady = false;
				objectsToRender.erase(y * WWidth + x);
				/*for (int j = 0; j < objectsToRender.size(); j++)
				{
					if (objectsToRender[j]->cell == int2(x, y))
					{
						objectsToRender.erase(objectsToRender.begin() + j);
						break;
					}
				}
				*/
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
					(*grid)[activeCells[j].x][activeCells[j].y]->status = LOADING;
					cellIndex = j;
					break;
				}
			}
			if (cellIndex == -1) // return if no grid cells needs to be loaded
				return;

			cellRender* cell = new cellRender();
			cell->objects = new std::vector<std::shared_ptr<Mesh>>();
			cell->thread = tID;
			objectsToRender[activeCells[cellIndex].y * WWidth + activeCells[cellIndex].x] = cell;

			createThread(threads[tID], (*grid)[activeCells[cellIndex].x][activeCells[cellIndex].y]->objectList, cell->objects, tID, activeCells[cellIndex].x, activeCells[cellIndex].y);
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
			int index = threadData[i].cellY * WWidth + threadData[i].cellX;
			//// Since the thread has finished it has loaded all of the meshes in one grid cell, however, we need to find which cell the thread is responsible for
			//for (int j = 0; j < objectsToRender.size(); j++)
			//{
			//	if (objectsToRender[j]->thread == i && objectsToRender[j]->isReady == false)
			//		index = j;
			//}

			// mikaels funtion, set command list such that we can execute it and change resource state.
			for (unsigned int k = 0; k < (*objectsToRender[index]->objects).size(); k++)
			{
				renderer->setDirectList((*objectsToRender[index]->objects)[k].get(), MAIN_THREAD);
			}
			renderer->executeDirectCommandList(i);
			renderer->signalDirect(FENCEDONE, i);

			// might as well wait for it when we're testing.
			renderer->waitForDirect(FENCEDONE, INFINITY, i);
			renderer->resetDirectCommandList(i);
	
			// tell the renderer that the list is ready to draw
			(*grid)[threadData[i].cellX][threadData[i].cellY]->status = LOADED;
			objectsToRender[index]->isReady = true;
			idleThreads[i] = true;
		}
	}
}

void threadDataCollecting(bool* work)
{
	ofstream file;
	int i = 0;
	file.open("data.txt", ios_base::app);
	while (work[0])
	{
		file << activeCells.size() << endl;
		file.flush();
		this_thread::sleep_for(chrono::seconds(1));
	}
	file.close();
	return;
}
#undef main
int main(int argc, char *argv[])
{
	oldCamPos.x = 0;
	oldCamPos.y = 0;
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

	bool work[1];
	work[0] = true;

	dataCollector = thread(threadDataCollecting, work);
	//D3D12Timer timer(renderer->getDevice().Get());
	
	//(*grid)[0].size();
	//Vector3 pos = (*grid)[0][0]->objectList[0]->position;
	//initialiseTestbench();
	run();
	shutdown();
	work[0] = false;
	dataCollector.join();
	return 0;
};
