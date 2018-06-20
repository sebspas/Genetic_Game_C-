#include "stdafx.h"
#include "TrainingData.h"
#include <string>
#include <sstream>

using namespace NeuralNetwork;

void TrainingData::getTopology(std::vector<unsigned> &topology)
{
	std::string line;
	std::string label;

	getline(trainingDataFile, line);
	std::stringstream ss(line);
	ss >> label;
	if (this->isEof() || label.compare("topology:") != 0) {
		abort();
	}

	while (!ss.eof()) {
		unsigned n;
		ss >> n;
		topology.push_back(n);
	}

	return;
}

TrainingData::TrainingData(const std::string filename)
{
	trainingDataFile.open(filename.c_str());
}

unsigned TrainingData::get_next_inputs(std::vector<double> &input_vals)
{
	input_vals.clear();

	std::string line;
	getline(trainingDataFile, line);
	std::stringstream ss(line);

	std::string label;
	ss >> label;
	if (label.compare("in:") == 0) {
		double oneValue;
		while (ss >> oneValue) {
			input_vals.push_back(oneValue);
		}
	}

	return input_vals.size();
}

unsigned TrainingData::get_target_outputs(std::vector<double> &target_output_vals)
{
	target_output_vals.clear();

	std::string line;
	getline(trainingDataFile, line);
	std::stringstream ss(line);

	std::string label;
	ss >> label;
	if (label.compare("out:") == 0) {
		double one_value;
		while (ss >> one_value) {
			target_output_vals.push_back(one_value);
		}
	}

	return target_output_vals.size();
}

