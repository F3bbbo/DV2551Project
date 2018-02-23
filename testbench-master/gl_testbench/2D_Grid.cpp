#include "2D_Grid.h"

twoDgrid::twoDgrid()
{
	createtwoDgrid(1, 1);
}

twoDgrid::~twoDgrid()
{
}

void twoDgrid::createtwoDgrid(int height, int width)
{
	std::vector<Object> testting;
	Object test;
	test.Name = "yolo";
	twodtest.push_back(testting);
	twodtest[0].push_back(test);
	std::cout << "test" << std::endl;
	std::string testing = twodtest[0][0].Name;
}
