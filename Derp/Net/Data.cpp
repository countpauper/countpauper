#include "Net.h"

namespace Net
{
	namespace Data
	{

		Base::Base( const Eigen::VectorXd& activation) :
			activation(activation)
		{
		}
		Input::Input(const Eigen::VectorXd& activation) :
			Base(activation)
		{
		}
		Output::Output(const Eigen::VectorXd& activation) :
			Base(activation)
		{
		}

		double Output::MeanSquaredError(const Output& other) const
		{
			Eigen::VectorXd error = activation - other.activation;
			return error.dot(error);
		}
		double Outputs::MeanSquaredError(const Outputs& other) const
		{
			double totalE = 0;
			for (const auto& output : *this)
				totalE += output.second.MeanSquaredError(other.at(output.first));
			return totalE;
		}

		Sample::Sample(const Inputs& inputs, const Outputs& outputs) :
			inputs(inputs),
			outputs(outputs)
		{
		}
	}
}
