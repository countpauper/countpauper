#include "Function.h"
#include "Math.h"

namespace Net
{
namespace Activation
{
	Eigen::VectorXd Boolean::Activate(const Eigen::VectorXd& excitation) const
	{
		return excitation.unaryExpr([](double e) { return e > 0 ? 1.0 : 0.0; });
	}

	Function* Boolean::Copy() const
	{
		return new Boolean(*this);
	}

	Eigen::VectorXd Stochastic::Activate(const Eigen::VectorXd& excitation) const
	{
		return excitation.unaryExpr([](double e)
		{
			return p(sigmoid(e)) ? 1.0 : 0.0;
		});
	}

	Function* Stochastic::Copy() const
	{
		return new Stochastic(*this);
	}

	Eigen::VectorXd Linear::Activate(const Eigen::VectorXd& excitation) const
	{
		return excitation;
	}

	Function* Linear::Copy() const
	{
		return new Linear(*this);
	}

	Eigen::VectorXd Sigmoid::Activate(const Eigen::VectorXd& excitation) const
	{
		return excitation.unaryExpr(std::ptr_fun(sigmoid));
	}

	Function* Sigmoid::Copy() const
	{
		return new Sigmoid(*this);
	}
}
}
