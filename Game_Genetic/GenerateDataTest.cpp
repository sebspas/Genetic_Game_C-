// Game_Genetic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ostream>
#include <iostream>

int main()
{
	std::cout << "topology: 2 4 1" << std::endl;

	for (auto i = 2000; i >= 0; --i)
	{
		const auto n1 = static_cast<int>(2.0 * rand() / double(RAND_MAX));
		const auto n2 = static_cast<int>(2.0 * rand() / double(RAND_MAX));
		const auto t = n1 ^ n2;

		std::cout << "in: " << n1 << ".0 " << n2 << ".0 " << std::endl;
		std::cout << "out: " << t << ".0 " << std::endl;
	}

	return 0;
}

