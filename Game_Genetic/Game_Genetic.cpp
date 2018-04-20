// Game_Genetic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Net.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <string>
#include "TrainingData.h"

void show_vector_vals(const std::string label, std::vector<double> &v)
{
	std::cout << label << " ";
	for (unsigned i = 0; i < v.size(); ++i) {
		std::cout << v[i] << " ";
	}

	std::cout << std::endl;
}

void Generate_training_data_XOR()
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


int main()
{
	Generate_training_data_XOR();

	TrainingData train_data("trainingData.txt");

	std::vector<unsigned> topology;
	train_data.getTopology(topology);

	Net myNet(topology);

	std::vector<double> input_vals, target_vals, result_vals;
	auto training_pass = 0;

	while (!train_data.isEof()) {
		++training_pass;
		std::cout << std::endl << "Pass " << training_pass;

		// Get new input data and feed it forward:
		if (train_data.get_next_inputs(input_vals) != topology[0]) {
			break;
		}

		show_vector_vals(": Inputs:", input_vals);
		myNet.feed_forward(input_vals);

		// Collect the net's actual output results:
		myNet.get_results(result_vals);
		show_vector_vals("Outputs:", result_vals);

		// Train the net what the outputs should have been:
		train_data.get_target_outputs(target_vals);
		show_vector_vals("Targets:", target_vals);
		assert(target_vals.size() == topology.back());

		myNet.back_prop(target_vals);

		// Report how well the training is working, average over recent samples:
		std::cout << "Net recent average error: "
			<< myNet.get_recent_average_error() << std::endl;
	}

	std::cout << std::endl << "Done" << std::endl;

	char i;
	std::cout << "Press anything to quit..." << std::endl;
	std::cin >> i;

    return 0;
}

