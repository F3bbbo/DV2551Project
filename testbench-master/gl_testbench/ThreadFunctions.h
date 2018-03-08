#pragma once
#ifndef THREADFUNCTIONS_H
#include <Windows.h>
#include <process.h>
#include "Grid.h"
#include "Mesh.h"
#include "MeshReader.h"
#include "DirectX12\Directx12Renderer.h"
#include "DirectX12\Texture2DDX12.h"
#include "Technique.h"
#include "Defines.h"

struct threadinfo
{
	int size;
	Object** data;
	int key;
	std::vector<std::shared_ptr<Mesh>>* meshes;
	MeshReader *reader;
	DirectX12Renderer* renderer;
	std::shared_ptr<Technique>* technique;
};


unsigned int __stdcall  threadfunctionloadingdata(void* data);

#endif

