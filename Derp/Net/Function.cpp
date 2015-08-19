#include "stdafx.h"
#include "Function.h"
#include "Math.h"

namespace Net
{

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
