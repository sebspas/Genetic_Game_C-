#pragma once
#include <vector>

class Neuron;

typedef std::vector<Neuron> Layer;

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

	void set_output_value(const double val) { output_value = val; }
	double get_output_value() const { return output_value; }

	void feed_forward(const Layer& prev_layer);
	void calculate_output_gradient(const double targets_value);
	double sum_oow(const Layer & next_layer) const;
	void calculate_hidden_gradient(const Layer& next_layer);
	void update_input_weight(Layer& prev_layer);

private:
	unsigned my_index;	
	double output_value;
	std::vector<Connection> output_weights;
	double gradient;

	static double eta; // [0.0 ... 1.0] overall net training rate
	static double alpha; // [0.0 ... n] multiplier of last weight change

	inline static double random_weight(void) { return rand() / double(RAND_MAX); }
	static  double transfer_function(const double x);
	static double transfer_function_derivative(const double x);

};
