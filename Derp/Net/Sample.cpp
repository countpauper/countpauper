#include "stdafx.h"
#include "Sample.h"

namespace Net
{
	Sample::Sample(const Eigen::VectorXd& activation) :
		activation(activation)
	{
	}

	Eigen::VectorXd Sample::Activation(size_t from, size_t count) const
	{
		return activation.segment(from, count);
	}
}
