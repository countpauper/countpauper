#include "Data.h"

namespace Net
{
	namespace Data
	{

		Base::Base(Layer::Id layer, const Eigen::VectorXd& activation) :
			layer(layer),
			activation(activation)
		{
		}
		Input::Input(Layer::Id layer, const Eigen::VectorXd& activation) :
			Base(layer, activation)
		{
		}
		Output::Output(Layer::Id layer, const Eigen::VectorXd& activation) :
			Base(layer, activation)
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
				totalE += output.MeanSquaredError(other[output.layer]);
			return totalE;
		}

		const Output& Outputs::operator[](Layer::Id id) const
		{
			const_iterator it = std::find_if(cbegin(), cend(), [id](const Output& output){ return output.layer == id;  });
			if (it != end())
				return *it;
			else
				throw std::out_of_range("Output not found");
		}


		Sample::Sample(const Inputs& inputs, const Outputs& outputs) :
			inputs(inputs),
			outputs(outputs)
		{
		}
	}
}
