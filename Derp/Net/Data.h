#pragma once
#include <Eigen/Dense>

namespace Net
{
	class Sample
	{
	public:
		Sample(const Eigen::VectorXd& activation);
		Eigen::VectorXd Activation(size_t from, size_t count) const;
	protected:
		Eigen::VectorXd activation;
	};
}
