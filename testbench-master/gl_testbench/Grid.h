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
	Object(float* pos, float* scale, float* rot, std::string texture)
	{
		position = Vector3(pos);
		this->scale = Vector3(scale);
		rotation = Vector3(rot);
		textureFileName = texture;
	}
	std::string Name; //Object Name 
	std::string textureFileName;
	DirectX::SimpleMath::Vector3 position; // Object Position
	DirectX::SimpleMath::Vector3 scale; // Object Scale
	DirectX::SimpleMath::Vector3 rotation; // Object rotation
};

struct cellInfo
{
	std::vector<Object*> objectList;
	int status; // 0: not loaded, 1: loading, 2: loaded and ready to draw
};
class Grid
{

private:
//	std::vector<Object> test;
	std::vector<std::vector<cellInfo*>> ObjectGrid;
	int width, height;
public:
	void addMesh(int x, int y, Object* mesh);
	std::vector <cellInfo*>operator [](int y) const { return ObjectGrid[y];}
	std::vector <cellInfo*>&operator [](int y) { return ObjectGrid[y]; }
	Grid();
	~Grid();
	std::vector<Object*> getObject(int width, int height);
	void createGrid(int gridHeight, int gridWidth);

};


