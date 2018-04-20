#include "stdafx.h"
#include "Neuron.h"

double Neuron::eta = 0.15;
double Neuron::alpha = 0.5;

Neuron::Neuron(const unsigned nb_output, const unsigned index)
{
	for (unsigned c = 0; c < nb_output; ++c)
	{
		output_weights.push_back(Connection());
		output_weights.back().weight = random_weight();
	}

	my_index = index;
}

Neuron::~Neuron()
{
}

void Neuron::feed_forward(const Layer& prev_layer)
{
	double sum = 0.0;

	// sum
	for (unsigned n = 0; n < prev_layer.size(); ++n)
	{
		sum += prev_layer[n].get_output_value() * 
			prev_layer[n].output_weights[my_index].weight;
	}

	output_value = Neuron::transfer_function(sum);
}

void Neuron::calculate_output_gradient(const double targets_value)
{
	const auto delta = targets_value - output_value;
	gradient = delta * Neuron::transfer_function_derivative(output_value);
}

double Neuron::sum_oow(const Layer& next_layer) const
{
	auto sum = 0.0;

	for (unsigned n = 0; n < next_layer.size() - 1; ++n)
	{
		sum += output_weights[n].weight * next_layer[n].gradient;
	}

	return sum;
}

void Neuron::calculate_hidden_gradient(const Layer& next_layer)
{
	const auto dow = sum_oow(next_layer);
	gradient = dow * Neuron::transfer_function_derivative(output_value);
}

void Neuron::update_input_weight(Layer& prev_layer)
{
	for (unsigned n = 0; n < prev_layer.size(); ++n)
	{
		Neuron &neuron = prev_layer[n];
		auto old_delta_weight = neuron.output_weights[my_index].delta_weight;
		auto new_delta_weight = eta * neuron.output_value * gradient + alpha * old_delta_weight;

		neuron.output_weights[my_index].delta_weight = new_delta_weight;
		neuron.output_weights[my_index].weight += new_delta_weight;
	}
}

double Neuron::transfer_function(const double x)
{
	// hyperbolik
	// tanh - output range [-1.0 .... 1.0]
	return tanh(x);
}

double Neuron::transfer_function_derivative(const double x)
{
	// derivative of tanh
	// 1 - x²
	return 1.0 - x * x;
}
