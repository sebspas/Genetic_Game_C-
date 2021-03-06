#include "stdafx.h"
#include "Net.h"
#include <iostream>
#include <cassert>
#include <string>
#include "TrainingData.h"

using namespace NeuralNetwork;

double Net::recent_average_smoothing_factor = 100.0; // Number of training samples to average over

void show_vector_vals(const std::string label, std::vector<double> &v)
{
	std::cout << label << " ";
	for (unsigned i = 0; i < v.size(); ++i) {
		std::cout << v[i] << " ";
	}

	std::cout << std::endl;
}

Net::Net(const std::vector<unsigned>& topology)
{
	error = 0.0;
	recent_average_error = 0.0;
	net_topology = topology;

	const auto num_layers = topology.size();

	for (unsigned layer_num = 0; layer_num < num_layers; ++layer_num)
	{
		layers.push_back(Layer());
		const auto nb_output = layer_num == topology.size() - 1 ? 0 : topology[layer_num + 1];

		// <= because we also add a biais neuron
		for (unsigned neuron_num = 0; neuron_num <= topology[layer_num]; ++neuron_num)
		{
			// we get the last element (so the current layer)
			layers.back().push_back(Neuron(nb_output, neuron_num));
			std::cout << "Made a neuron !" << std::endl;
		}

		// set the biais neuron output value to 1.0
		layers.back().back().set_output_value(1.0);
	}
}

Net::~Net()
{
}

void Net::feed_forward(const std::vector<double>& inputs_values)
{
	// check if the inputs values is the same than the input layer minus the biais
	assert(inputs_values.size() == layers[0].size() - 1);

	// set the input value in the input neuron
	for (unsigned i = 0; i < inputs_values.size(); ++i)
	{
		layers[0][i].set_output_value(inputs_values[i]);
	}

	// forward propagation
	for (unsigned layer_num = 1; layer_num < layers.size(); ++layer_num)
	{
		Layer& prev_layer = layers[layer_num - 1];
		for (unsigned neuron = 0; neuron < layers[layer_num].size() - 1; ++neuron)
		{
			layers[layer_num][neuron].feed_forward(prev_layer);
		}
	}
}

void Net::back_prop(const std::vector<double>& targets_values)
{
	// Calculate overall net error
	Layer& output_layer = layers.back();
	error = 0.0;

	for (unsigned n = 0; n < output_layer.size() - 1; ++n)
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

	for (unsigned n = 0; n < output_layer.size() - 1; ++n)
	{
		output_layer[n].calculate_output_gradient(targets_values[n]);
	}

	// Calculate gradients on hidden layers

	for (unsigned layer_num = layers.size() - 2; layer_num > 0; --layer_num)
	{
		Layer& hidden_layer = layers[layer_num];
		Layer& next_layer = layers[layer_num + 1];

		for (unsigned n = 0; n < hidden_layer.size(); ++n)
		{
			hidden_layer[n].calculate_hidden_gradient(next_layer);
		}
	}

	// Update connection weight depending on the error
	for (unsigned layer_num = layers.size() - 1; layer_num > 0; --layer_num)
	{
		Layer& layer = layers[layer_num];
		Layer& prev_layer = layers[layer_num - 1];

		for (unsigned n = 0; n < layer.size() - 1; ++n)
		{
			layer[n].update_input_weight(prev_layer);
		}
	}
}

void Net::get_results(std::vector<double>& results_values) const
{
	results_values.clear();

	for (unsigned n = 0; n < layers.back().size() - 1; ++n)
	{
		results_values.push_back(layers.back()[n].get_output_value());
	}
}

void Net::trainNetwork(TrainingData* train_data)
{
	std::vector<double> input_vals, target_vals, result_vals;
	auto training_pass = 0;

	while (!train_data->isEof()) {
		++training_pass;
		std::cout << std::endl << "Pass " << training_pass;

		// Get new input data and feed it forward:
		if (train_data->get_next_inputs(input_vals) != net_topology[0]) {
			break;
		}

		show_vector_vals(": Inputs:", input_vals);
		feed_forward(input_vals);

		// Collect the net's actual output results:
		get_results(result_vals);
		show_vector_vals("Outputs:", result_vals);

		// Train the net what the outputs should have been:
		train_data->get_target_outputs(target_vals);
		show_vector_vals("Targets:", target_vals);
		assert(target_vals.size() == net_topology.back());

		back_prop(target_vals);

		// Report how well the training is working, average over recent samples:
		std::cout << "Net recent average error: " << get_recent_average_error() << std::endl;
	}

	std::cout << std::endl << "Training over!" << std::endl;
}