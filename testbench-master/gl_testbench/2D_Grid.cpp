#include "2D_Grid.h"

twoDgrid::twoDgrid()
{
	createtwoDgrid(2, 4);
}

twoDgrid::~twoDgrid()
{
}

void twoDgrid::createtwoDgrid(int height, int width)
{
//	std::vector<std::vector<Object>> testtting;
//	std::vector<Object> testting;
//	Object test;
//	test.Name = "yolo";
//	twodtest.push_back(testtting);
//	twodtest[0].push_back(testting);
//	twodtest[0][0].push_back(test);
//	std::cout << "test" << std::endl;
//	std::string testing = twodtest[0][0][0].Name;
	std::vector<std::vector<Object>> Xaxel;
	std::vector<Object> Yaxel;
	for (int i = 0; i < width;i++)
	{
		twodgrid.push_back(Xaxel);
		for (int j = 0; j < height; j++)
		{
			twodgrid[i].push_back(Yaxel);
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
