#pragma once
#include <fstream>
#include <vector>

namespace NeuralNetwork {
	class TrainingData
	{
	public:
		TrainingData(const std::string filename);
		bool isEof(void) const { return trainingDataFile.eof(); }
		void getTopology(std::vector<unsigned> &topology);

		// Returns the number of input values read from the file:
		unsigned get_next_inputs(std::vector<double> &input_vals);
		unsigned get_target_outputs(std::vector<double> &target_output_vals);

	private:
		std::ifstream trainingDataFile;
	};
}
