#pragma once

#include <Eigen/Dense>
#include <vector>
#include <memory>

#include "Activation.h"
#include "Function.h"

namespace Net
{
	namespace Connection
	{
		class Base;
	}
	namespace Layer
	{
		class Base
		{
		public:
			typedef std::vector<Connection::Base*> Connections;
			Base();
			Base(size_t units, const Function& function);
			virtual ~Base() = default;
			void Connect(Connection::Base& connection);
			size_t Size() const;
			const Eigen::VectorXd& Bias() const { return bias; }
			const Connections& GetConnections() const { return connections; }
			const Connection::Base& operator[](unsigned index) const { return *connections[index]; }
			Connection::Base& operator[](unsigned index) { return *connections[index]; }
			void Reset(double mean, double sigma);
			const Function& GetFunction() const { return *function.get(); }
		private:
			size_t units;
			Connections connections;
			std::unique_ptr<Function> function;
			Eigen::VectorXd bias;

			friend std::ostream& operator<< (std::ostream& stream, const Net::Layer::Base& layer);
			friend std::istream& operator>> (std::istream& stream, Net::Layer::Base& layer);
		};

		class Input : public Base
		{
		public:
			Input() = default;
			Input(size_t units);
		};

		class Output : public Base
		{
		public:
			Output() = default;
			Output(size_t units, const Function& function);
		};

		class Visible : public Base
		{
		public:
			Visible() = default;
			Visible(size_t units, const Function& function);
		};

		class Hidden : public Base
		{
		public:
			Hidden() = default;
			Hidden(size_t units, const Function& function);

		};
	}
}
