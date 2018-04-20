// Game_Genetic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Net.h"
#include <vector>
#include <iostream>

int main()
{
	std::vector<unsigned> topology;
	topology.push_back(3);
	topology.push_back(2);
	topology.push_back(1);
	Net my_net(topology);

	std::vector<double> inputs_values;
	my_net.feed_forward(inputs_values);

	std::vector<double> targets_values;
	my_net.back_prop(targets_values);

	std::vector<double> results_values;
	my_net.get_results(results_values);

	char i;
	std::cout << "Press anything to quit..." << std::endl;
	std::cin >> i;

    return 0;
}

