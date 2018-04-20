#include "stdafx.h"
#include "Net.h"
#include <iostream>
#include <cassert>

Net::Net(const std::vector<unsigned>& topology)
{
	const auto num_layers = topology.size();

	for (auto layer_num = 0; layer_num < num_layers; ++layer_num)
	{
		layers.push_back(Layer());
		const auto nb_output = layer_num == topology.size() - 1 ? 0 : topology[layer_num + 1];

		// <= because we also add a biais neuron
		for (auto neuron_num = 0; neuron_num <= topology[layer_num]; ++neuron_num)
		{
			// we get the last element (so the current layer)
			layers.back().push_back(Neuron(nb_output, neuron_num));
			std::cout << "Made a neuron !" << std::endl;
		}
	}

	// set the biais neuron output value to 1.0
	layers.back().back().set_output_value(1.0);
}

Net::~Net()
{
}

void Net::feed_forward(const std::vector<double>& inputs_values)
{
	// check if the inputs values is the same than the input layer minus the biais
	assert(inputs_values.size() == layers[0].size() - 1);

	// set the input value in the input neuron
	for (auto i = 0; i < inputs_values.size(); ++i)
	{
		layers[0][i].set_output_value(inputs_values[i]);
	}

	// forward propagation
	for (auto layer_num = 1; layer_num < layers.size(); ++layer_num)
	{
		Layer& prev_layer = layers[layer_num - 1];
		for (auto neuron = 0; neuron < layers[layer_num].size() - 1; ++neuron)
		{
			layers[layer_num][neuron].feed_forward(prev_layer);
		}
	}
}

void Net::back_prop(const std::vector<double>& targets_values)
{
	// Calculate overall net error
	auto output_layer = layers.back();
	error = 0.0;

	for (auto n = 0; n < output_layer.size() - 1; ++n)
	{
		double delta = targets_values[n] - output_layer[n].get_output_value();
		error += delta * delta;
	}

	error /= output_layer.size() - 1;
	error = sqrt(error);

	// average mesurement
	recent_average_error = (recent_average_error * recent_average_smoothing_factor + error)
	/ (recent_average_smoothing_factor + 1.0);

	// Calculate output layer gradients

	for (auto n = 0; n < output_layer.size() - 1; ++n)
	{
		output_layer[n].calculate_output_gradient(targets_values[n]);
	}

	// Calculate gradients on hidden layers

	for (int layer_num = layers.size() - 2; layer_num > 0; --layer_num)
	{
		auto hidden_layer = layers[layer_num];
		auto next_layer = layers[layer_num + 1];

		for (auto n = 0; n < hidden_layer.size(); ++n)
		{
			hidden_layer[n].calculate_hidden_gradient(next_layer);
		}
	}

	// Update connection weight depending on the error
	for (int layer_num = layers.size() - 1; layer_num >= 0; --layer_num)
	{
		auto layer = layers[layer_num];
		auto prev_layer = layers[layer_num - 1];

		for (auto n = 0; n < layer.size() - 1; ++n)
		{
			layer[n].update_input_weight(prev_layer);
		}
	}
}

void Net::get_results(std::vector<double>& results_values) const
{
	results_values.clear();

	for (auto n = 0; n < layers.back().size() - 1; ++n)
	{
		results_values.push_back(layers.back()[n].get_output_value());
	}
}

