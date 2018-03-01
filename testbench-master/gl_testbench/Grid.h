#pragma once
#include <iostream>
#include <d3d12.h>
#include <dxgi1_5.h>
#include <SimpleMath.h>
#include <vector>

#define cellWidth 1
#define cellHeight 1

using namespace DirectX::SimpleMath;

struct Object
{
	Object(float* pos, float* scale, float* rot)
	{
		position = Vector3(pos);
		this->scale = Vector3(scale);
		rotation = Vector3(rot);
	}
	std::string Name; //Object Name 
	DirectX::SimpleMath::Vector3 position; // Object Position
	DirectX::SimpleMath::Vector3 scale; // Object Scale
	DirectX::SimpleMath::Vector3 rotation; // Object rotation
};


class Grid
{

private:
//	std::vector<Object> test;
	std::vector<std::vector<std::vector<Object*>>> ObjectGrid;
	int width, height;
public:
	void addMesh(int x, int y, Object* mesh);
	Grid();
	~Grid();
	void createGrid(int gridHeight, int gridWidth);

};


