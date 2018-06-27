#include "stdafx.h"
#include "SnakeRecorder.h"
#include <fstream>
#include "SnakeGame.h"


SnakeRecorder::SnakeRecorder(std::string my_data_file)
{	
	myfile.open(my_data_file);
	myfile << "topology: 5 10 3" << std::endl;
}

SnakeRecorder::~SnakeRecorder()
{
	myfile.close();
}

void SnakeRecorder::writeData(int obj_left, int obj_front, int obj_right, int deltaFoodX, int deltaFoodY, int forward)
{
	myfile << "in: " << obj_left << " " << obj_front << " " << obj_right << " " << deltaFoodX << " " << deltaFoodY << std::endl;

	int outL, outForward, outR;
	outR = outForward = outL = 0;

	if (forward == Snake::RIGHT)
	{
		outR = 1;
	} else if (forward == Snake::LEFT)
	{
		outL = 1;
	} else if (forward == Snake::UP)
	{
		outForward = 1;
	}

	myfile << "out: " << outL << " " << outForward << " " << outR << std::endl;
}
