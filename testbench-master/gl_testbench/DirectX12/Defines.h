#pragma once
#define DEPTH_STENCIL_FORMAT DXGI_FORMAT_D24_UNORM_S8_UINT
#define SWAP_CHAIN_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM
#define SWAP_BUFFER_COUNT 2
#define MAIN_THREAD NUMBER_OF_THREADS - 1
#define NUMBER_OF_LOADING_THREADS NUMBER_OF_THREADS - 1
#define THREAD_ID 0

//Fence Defines
#define FENCEDONE 1
#define FENCEBUSY 0



//movementspeed
#define MOVEMENTSPEED 1
#define ROTATIONSPEED 0.001


//DataCollecter
#define SAMPLETIME 10

//Testing defines
#define NUMBER_OF_THREADS 7
#define NROFTREES 4
#define LOADINGTHRESHOLD 4000
#define NEARPLANE 0.1f
#define FARPLANE 10000.f
#define GRIDWIDTH 100
#define GRIDHEIGHT 100
#define cellWidth 1000
#define cellHeight 1000

#define MODELNAME "Models/18kTree.fbx"
#define TEXTURENAME "Models/PolyTreeTexture512.png"
