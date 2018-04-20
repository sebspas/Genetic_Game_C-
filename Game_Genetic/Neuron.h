#pragma once
#include <vector>
#include "Net.h"

struct Connection
{
	double weight;
	double delta_weight;
};

class Neuron
{
public:
	explicit Neuron(unsigned nb_output, unsigned index);
	~Neuron();

	void set_output_value(double val) { output_value = val; }
	double get_output_value() const { return output_value; }

	void feed_forward(const Layer& prev_layer);

private:
	unsigned my_index;	
	double output_value;
	std::vector<Connection> output_weights;

	inline static double random_weight(void) { return rand() / double(RAND_MAX); }
	static  double transfer_function(double x);
	static double transfer_function_derivative(double x);

};

