#pragma once
#include <string>
#include <fstream>

class SnakeRecorder
{
	std::ofstream myfile;

public:
	SnakeRecorder(std::string my_data_file);
	~SnakeRecorder();

	/**
	 * Obj : 
	 *	-1 wall or tail
	 *	 0 nothing
	 *	 1 food
	 * Output : direction see SnakeGame.h
	 */
	void writeData(int obj_left, int obj_right, int obj_front, int output);
};
