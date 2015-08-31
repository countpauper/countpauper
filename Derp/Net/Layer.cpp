#include <boost/format.hpp>
#include "Layer.h"
#include "Math.h"
#include "Connection.h"
#include "Function.h"
#include "IO.h"

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



		const IO::version Base::version = 1;

		std::unique_ptr<Activation::Function> FunctionFactory(const std::string& typeName)
		{
			if (typeName == "null")
				return nullptr;
			if (typeName == "Net::Activation::Boolean")
				return std::make_unique<Activation::Boolean>();
			else if (typeName == "Net::Activation::Linear")
				return std::make_unique<Activation::Linear>();
			else if (typeName == "Net::Activation::Sigmoid")
				return std::make_unique<Activation::Sigmoid>();
			else if (typeName == "Net::Activation::Stochastic")
				return std::make_unique<Activation::Stochastic>();
			else
				throw std::domain_error((boost::format("Syntax error, unknown activation function = '%s'.") % typeName).str());
		}

		std::ostream& operator<< (std::ostream& stream, const Base& layer)
		{
			stream << layer.version << IO::separator;
			stream << layer.Size() << IO::separator;
			stream << IO::classname_ptr(layer.function.get()) << std::endl;
			stream << layer.bias.transpose() << std::endl;
			return stream;
		}


		std::istream& operator>> (std::istream& stream, Base& layer)
		{
			IO::version v;
			stream >> v;
			if (v >= 1)
			{
				std::string functionName;
				size_t units;
				stream >> units >> functionName;
				layer.function = std::move(FunctionFactory(functionName));
				layer.bias = Eigen::VectorXd(units);
				stream >> layer.bias;
			}

			return stream;
		}
	}
}
