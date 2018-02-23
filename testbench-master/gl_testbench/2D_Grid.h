#pragma once
#include <iostream>
#include <d3d12.h>
#include <dxgi1_5.h>
#include <DirectXMath.h>
#include <vector>
struct Object
{
	std::string Name; //Object Name 
	DirectX::XMFLOAT3 Position; // Object Position
	DirectX::XMFLOAT3 Scale; // Object Scale
	float rotation; // Object rotation

};


class twoDgrid
{

private:
//	std::vector<Object> test;
	std::vector<std::vector<Object>> twodtest;
public:
	twoDgrid();
	~twoDgrid();
	void createtwoDgrid(int height, int width);

};


