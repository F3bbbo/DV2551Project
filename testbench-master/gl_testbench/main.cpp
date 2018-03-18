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

vector<std::shared_ptr<Material>> materials;
vector<std::shared_ptr<Technique>> techniques;

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
bool changedState[NUMBER_OF_LOADING_THREADS];
HANDLE threads[NUMBER_OF_LOADING_THREADS];
vector<int2> activeCells;
vector<vector<int2>> activeCells2;
threadinfo threadData[NUMBER_OF_LOADING_THREADS];

const int2 gridStart = { -5, -5 };

thread dataCollector;

std::chrono::system_clock::time_point timer;
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
//	deltatimeGlobale = deltaTime;
	// moving average window of WINDOWS_SIZE
	lastSum -= avg[loop];
	lastSum += deltaTime;
	avg[loop] = deltaTime;
	loop = (loop + 1) % WINDOW_SIZE;
	gLastDelta = (lastSum / WINDOW_SIZE);
	deltatimeGlobale = gLastDelta;
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

		auto stamp = std::chrono::system_clock::now();
		chrono::duration<float> elapsedTime = stamp - timer;
		if (elapsedTime.count() >= TIMELIMIT)
			break;
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

void shutdown() {
	delete meshReader;
	renderer->shutdown();
	delete renderer;
};
void fillCell(int x, int y, int amount)
{
	srand(time(NULL));

	float randNumbX, randNumbY;
	for (int i = 0; i < amount; i++)
	{
		randNumbX = (float)(rand() % cellWidth);
		randNumbY = (float)(rand() % cellHeight);
		float pos[3] = { x * cellWidth + randNumbX, 1, y * cellWidth + randNumbY };
		float scale[3] = { 1, 1, 1 };
		float rot[3] = { 0, 0, 0 };
		//Object* object = new Object(pos, scale, rot, "Models/NewLowPolyTree.fbx","Models/PolyTreeTexture.png");
		Object* object = new Object(pos, scale, rot, MODELNAME, TEXTURENAME);
		grid->addMesh(x, y, object);
	}

}


void setThreadData(HANDLE &threadHandle, std::vector<Object*> &objectList, std::vector<std::shared_ptr<Mesh>>* meshes, int threadKey, int cellX, int cellY)
{ 
	threadData[threadKey].cellX = cellX;
	threadData[threadKey].cellY = cellY;
	threadData[threadKey].size = objectList.size();
	threadData[threadKey].data = objectList.data();
	threadData[threadKey].key = threadKey;
	threadData[threadKey].meshes = meshes;
	threadData[threadKey].done = false;
	return;
}
void fillGrid()
{
	for (int x = 0; x < GRIDWIDTH; x++)
	{
		for (int y = 0; y < GRIDHEIGHT; y++)
			fillCell(x, y, NROFTREES);
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
	int xStartDist = min(max(0, int(((int)camPos.x - LOADINGTHRESHOLD) / (float)cellWidth)), GRIDWIDTH);
	int yStartDist = min(max(0, int(((int)camPos.y - LOADINGTHRESHOLD) / (float)cellHeight)), GRIDHEIGHT);
	int xEndDist = min(max(0, int(((int)camPos.x + LOADINGTHRESHOLD) / (float)cellWidth)), GRIDWIDTH);
	int yEndDist = min(max(0, int(((int)camPos.y + LOADINGTHRESHOLD) / (float)cellHeight)), GRIDHEIGHT);



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
				for (auto m : *(objectsToRender[y * GRIDWIDTH + x]->objects))
					m.~shared_ptr();
				delete objectsToRender[y * GRIDWIDTH + x];
				objectsToRender.erase(y * GRIDWIDTH + x);
				activeCells.erase(activeCells.begin() + i);
			}
		}
	}
}

void initiateThreads()
{
	for (int i = 0; i < NUMBER_OF_LOADING_THREADS; i++)
	{
		threadData[i] = threadinfo();
		threadData[i].reader = meshReader;
		threadData[i].renderer = renderer;
		threadData[i].technique = &triangleT;
		threadData[i].shutDown = false;

		threads[i] = (HANDLE)_beginthreadex(0, 0, &threadfunctionloadingdata, &threadData[i], 0, 0);
	}
}
void LaunchThreads()
{
	for (int tID = 0; tID < NUMBER_OF_LOADING_THREADS; tID++)
	{
		if (threadData[tID].done && changedState[tID])
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
			objectsToRender[activeCells[cellIndex].y * GRIDWIDTH + activeCells[cellIndex].x] = cell;

			setThreadData(threads[tID], (*grid)[activeCells[cellIndex].x][activeCells[cellIndex].y]->objectList, cell->objects, tID, activeCells[cellIndex].x, activeCells[cellIndex].y);
			ResumeThread(threads[tID]);
			changedState[tID] = false;
		}
	}
}

void CheckThreadLoading()
{
	for (int i = 0; i < NUMBER_OF_LOADING_THREADS; i++)
	{
		// check if a thread is done working and if it's data has had it's state changed and make ready for rendering
		if (threadData[i].done && !changedState[i])
		{
			int index = threadData[i].cellY * GRIDWIDTH + threadData[i].cellX;

			renderer->executeDirectCommandList(i);
			renderer->signalDirect(FENCEDONE, i);
	
			// tell the renderer that the list is ready to draw
			(*grid)[threadData[i].cellX][threadData[i].cellY]->status = LOADED;
			objectsToRender[index]->isReady = true;
			changedState[i] = true;
		}
	}
}

void threadDataCollecting(bool* work)
{
	ofstream file;
	int iteration = 0;
	int meshesLoaded;
	std::string fileName;
	fileName = "data" + std::to_string(NUMBER_OF_LOADING_THREADS) + ".txt";

	if (FILE *file = fopen(fileName.c_str(), "r")) {
		fclose(file);
		remove(fileName.c_str());
	}

	file.open(fileName, ios_base::app);
	file << "n\tMeshesToLoad\tMeshesLoaded\n";
	file.flush();
	
	std::string info = "";
	while (work[0])
	{
		file << iteration * SAMPLETIME / 1000.0f << '\t';
		meshesLoaded = 0;
		file << activeCells.size() * NROFTREES << '\t'; //amount of objects in the scene
		
		for (auto readyToRender : objectsToRender)
		{
			if (readyToRender.second != nullptr && readyToRender.second->isReady == true)
				meshesLoaded += readyToRender.second->objects->size();
		}
		
		file << meshesLoaded << '\n';
		
		file.flush();
		iteration++;
		this_thread::sleep_for(chrono::milliseconds(SAMPLETIME));
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
	grid->createGrid(GRIDWIDTH, GRIDHEIGHT);
	fillGrid();

	bool work[1];
	work[0] = true;

	initiateThreads();

	for (int i = 0; i < NUMBER_OF_LOADING_THREADS; i++)
		changedState[i] = true;

	dataCollector = thread(threadDataCollecting, work);
	timer = std::chrono::system_clock::now(); // start timer
	run();
	work[0] = false;
	dataCollector.join();
	for (int i = 0; i < NUMBER_OF_LOADING_THREADS; i++)
	{
		threadData[i].shutDown = true;
		ResumeThread(threads[i]);
	}
	WaitForMultipleObjects(NUMBER_OF_LOADING_THREADS, threads, true, INFINITE);
	shutdown();
	return 0;
}
