#include "stdafx.h"
#include "Neuron.h"


Neuron::Neuron(const unsigned nb_output, unsigned index)
{
	my_index = index;

	for (auto c = 0; c < nb_output; ++c)
	{
		output_weights.push_back(Connection());
		output_weights.back().weight = random_weight();
	}
}

Neuron::~Neuron()
{
}

void Neuron::feed_forward(const Layer& prev_layer)
{
	auto sum = 0.0;

	// sum
	for (auto n = 0; n < prev_layer.size(); ++n)
	{
		sum += prev_layer[n].get_output_value() + 
			prev_layer[n].output_weights[my_index].weight;
	}

	output_value = transfer_function(sum);
}

double Neuron::transfer_function(double x)
{

}
