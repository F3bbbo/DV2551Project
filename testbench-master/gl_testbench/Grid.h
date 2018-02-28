#pragma once
#include <iostream>
#include <d3d12.h>
#include <dxgi1_5.h>
#include <SimpleMath.h>
#include <vector>

#define cellWidth 1
#define cellHeight 1

struct Object
{
	std::string Name; //Object Name 
	DirectX::SimpleMath::Vector3 Position; // Object Position
	DirectX::SimpleMath::Vector3 Scale; // Object Scale
	DirectX::SimpleMath::Vector3 rotation; // Object rotation
};


class Grid
{

private:
//	std::vector<Object> test;
	std::vector<std::vector<std::vector<Object>>> ObjectGrid;
public:
	Grid();
	~Grid();
	void createGrid(int gridHeight, int gridWidth);

};


