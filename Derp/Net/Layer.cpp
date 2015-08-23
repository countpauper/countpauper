#include "stdafx.h"
#include "Layer.h"
#include "Math.h"
#include "Connection.h"

namespace Net
{
	namespace Layer
	{
		Base::Base() :
			units(0)
		{
		}

		Base::Base(size_t units, const Function& function) :
			units(units),
			bias(units),
			function(function.Copy())
		{
		}

		void Base::Connect(Connection::Base& connection)
		{
			if (&connection.A() == this)
				connections.push_back(&connection);
			else
				reverse_connections.push_back(&connection);
		}

		size_t Base::Size() const
		{
			return units;
		}

		void Base::Reset(double mean, double sigma)
		{
			bias.setConstant(mean);
			if (sigma != 0)
			{
				bias = bias.binaryExpr(Eigen::VectorXd::Constant(bias.size(), sigma), std::ptr_fun(normal_noise));
			}
		}

		const Function& Base::ChangeFunction(const Function& newFunction)
		{
			function.reset(newFunction.Copy());
			return GetFunction();
		}
		Input::Input(size_t units) :
			Base(units, Net::Linear())
		{
		}

		Output::Output(size_t units, const Function& function) :
			Base(units, function)
		{
		}

		Visible::Visible(size_t units, const Function& function) :
			Base(units, function)
		{
		}

		Hidden::Hidden(size_t units, const Function& function) :
			Base(units, function)
		{
		}
	}
}
