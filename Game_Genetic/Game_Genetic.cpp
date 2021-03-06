// Game_Genetic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Net.h"
#include <string>
#include "TrainingData.h"
#include <iostream>

void generate_training_data_xor()
{
	std::ofstream training_data_file;

	training_data_file.open("trainingData.txt");

	training_data_file << "topology: 2 4 1" << std::endl;

	for (auto i = 2000; i >= 0; --i)
	{
		const auto n1 = static_cast<int>(2.0 * rand() / double(RAND_MAX));
		const auto n2 = static_cast<int>(2.0 * rand() / double(RAND_MAX));
		const auto t = n1 ^ n2;

		training_data_file << "in: " << n1 << ".0 " << n2 << ".0 " << std::endl;
		training_data_file << "out: " << t << ".0 " << std::endl;
	}

	training_data_file.close();
}

/*
int main()
{
	generate_training_data_xor();

	NeuralNetwork::TrainingData train_data("trainingData.txt");

	std::vector<unsigned> topology;
	train_data.getTopology(topology);

	NeuralNetwork::Net myNet(topology);

	myNet.trainNetwork(&train_data);

	char i;
	std::cout << "Press anything to quit..." << std::endl;
	std::cin >> i;

    return 0;
}
*/