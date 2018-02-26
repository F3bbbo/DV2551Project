#include "Grid.h"

Grid::Grid()
{
	createGrid(1, 1);
}

Grid::~Grid()
{

}

void Grid::createGrid(int height, int width)
{
	std::vector<Object> testting;
	Object test;
	test.Name = "yolo";
	twodtest.push_back(testting);
	twodtest[0].push_back(test);
	std::cout << "test" << std::endl;
	std::string testing = twodtest[0][0].Name;
}
