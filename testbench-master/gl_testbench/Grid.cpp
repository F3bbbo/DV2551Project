#include "Grid.h"

Grid::Grid()
{
	createGrid(1, 1);
}

Grid::~Grid()
{

}

void Grid::createGrid(int gridHeight, int gridWidth)
{
	std::vector<std::vector<Object>> Xaxel;
	std::vector<Object> Yaxel;
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
