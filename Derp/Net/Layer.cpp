#include "Layer.h"
#include "Math.h"
#include "Connection.h"
#include "Function.h"

namespace Net
{
	namespace Activation { class Function; }
	namespace Layer
	{
		Base::Base()
		{
		}

		Base::Base(size_t units, const Activation::Function& function) :
			bias(units),
			function(function.Copy())
		{
		}

		Base::Base(const Base& other) :
			bias(other.bias),
			function(other.function->Copy()),
			connections()	// connections not RAII
		{
		}

		Base::~Base()
		{
		}

		void Base::Connect(Connection::Base& connection)
		{
			if (&connection.a == this)
				connections.push_back(connection);
			else
				reverse_connections.push_back(connection);
		}
		const Connection::Base& Base::operator[](unsigned index) const
		{ 
			return connections[index]; 
		}
		Connection::Base& Base::operator[](unsigned index)
		{ 
			return connections[index]; 
		}
		
		size_t Base::Size() const
		{
			return bias.size();
		}

		void Base::Reset(double mean, double sigma)
		{
			bias.setConstant(mean);
			if (sigma != 0)
			{
				bias = bias.binaryExpr(Eigen::VectorXd::Constant(bias.size(), sigma), std::ptr_fun(normal_noise));
			}
		}

		const Activation::Function& Base::ChangeFunction(const Activation::Function& newFunction)
		{
			function.reset(newFunction.Copy());
			return GetFunction();
		}
		Input::Input(size_t units) :
			Base(units, Net::Activation::Linear())
		{
		}

		Output::Output(size_t units, const Activation::Function& function) :
			Base(units, function)
		{
		}

		Visible::Visible(size_t units, const Activation::Function& function) :
			Base(units, function)
		{
		}

		Hidden::Hidden(size_t units, const Activation::Function& function) :
			Base(units, function)
		{
		}
	}
}
