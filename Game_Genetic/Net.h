#pragma once
#include <vector>
#include "Neuron.h"

typedef std::vector<Neuron> Layer;

class Net
{
public:
	explicit Net(const std::vector<unsigned> &topology);
	~Net();

	void feed_forward(const std::vector<double> &inputs_values);
	void back_prop(const std::vector<double> &targets_values);
	void get_results(std::vector<double> &results_values) const;

private:
	double error;
	double recent_average_error;
	double recent_average_smoothing_factor;
	std::vector<Layer> layers{}; // layers[layer_num][neuron_num]
};

