#include "stdafx.h"
#include "SnakeRecorder.h"
#include <fstream>
#include "SnakeGame.h"


SnakeRecorder::SnakeRecorder(std::string my_data_file)
{	
	myfile.open(my_data_file);
	myfile << "topology: 3 4 4" << std::endl;
}

SnakeRecorder::~SnakeRecorder()
{
	myfile.close();
}

void SnakeRecorder::writeData(int obj_left, int obj_right, int obj_up, int output)
{
	myfile << "in: " << obj_left << " " << obj_up << " " << obj_right << std::endl;

	int outL, outUp, outR, outDown;
	outR = outUp = outL = outDown = 0;

	if (output == Snake::RIGHT)
	{
		outR = 1;
	} else if (output == Snake::LEFT)
	{
		outL = 1;
	} else if (output == Snake::UP)
	{
		outUp = 1;
	} else if (output == Snake::DOWN)
	{
		outDown = 1;
	}

	myfile << "out: " << outL << " " << outUp << " " << outR << " " << outDown << std::endl;
}
