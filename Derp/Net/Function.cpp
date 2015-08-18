#include "stdafx.h"
#include "Function.h"

namespace Net
{
	double sigmoid(double e)
	{
		return 1.0 / (1.0 + exp(-e));
	}

	bool p(double chance)
	{
		double rnd = double(rand()) / double(RAND_MAX);
		return chance > rnd;
	}

	Eigen::VectorXd Boolean::Activate(const Eigen::VectorXd& excitation) const
	{
		return excitation.unaryExpr([](double e) { return e > 0 ? 1.0 : 0.0; });
	}


	Eigen::VectorXd Stochastic::Activate(const Eigen::VectorXd& excitation) const
	{
		return excitation.unaryExpr([](double e)
		{
			return p(sigmoid(e)) ? 1.0 : 0.0;
		});
	}

	Eigen::VectorXd Linear::Activate(const Eigen::VectorXd& excitation) const
	{
		return excitation;
	}

	Eigen::VectorXd Sigmoid::Activate(const Eigen::VectorXd& excitation) const
	{
		return excitation.unaryExpr(std::ptr_fun(sigmoid));
	}

}
