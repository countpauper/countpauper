#include "stdafx.h"
#include "Data.h"

namespace Net
{
	namespace Data
	{
		Input::Input(Layer::Id layer, const Eigen::VectorXd& activation) :
			layer(layer),
			activation(activation)
		{	
		}
		Output::Output(Layer::Id layer, const Eigen::VectorXd& activation) :
			layer(layer),
			activation(activation)
		{	
		}
		Sample::Sample(const Inputs& inputs, const Outputs& outputs) :
			inputs(inputs),
			outputs(outputs)
		{
		}
	}
}