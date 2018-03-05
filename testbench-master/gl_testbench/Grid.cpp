#include "Grid.h"

void Grid::addMesh(int x, int y, Object * mesh)
{
	this->ObjectGrid[x][y].push_back(mesh);
}

Grid::Grid()
{

}

Grid::~Grid()
{
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			for (int i = 0; i < ObjectGrid.size(); i++)
				delete ObjectGrid[x][y][i];
}

std::vector<Object*> Grid::getObject(int width, int height)
{
	
	return ObjectGrid[width][height];
}

void Grid::createGrid(int gridHeight, int gridWidth)
{
	width = gridWidth;
	height = gridHeight;

	std::vector<std::vector<Object*>> Xaxel;
	std::vector<Object*> Yaxel;
	for (int i = 0; i < gridWidth; i++)
	{
		ObjectGrid.push_back(Xaxel);
		for (int j = 0; j < gridHeight; j++)
		{
			ObjectGrid[i].push_back(Yaxel);
		}
	}
	/*
	Object test;
	test.Name = "yolo";
	twodgrid[0][0].push_back(test);
	test.Name = "neye";
	twodgrid[3][1].push_back(test);

	*/
}
