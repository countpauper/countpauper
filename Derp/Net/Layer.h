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
		typedef unsigned Id;

		class Base
		{
		public:
			typedef std::vector<Connection::Base*> Connections;
			Base();
			Base(size_t units, const Function& function);
			Base(const Base& other);
			virtual ~Base() = default;
			void Connect(Connection::Base& connection);
			size_t Size() const;
			const Eigen::VectorXd& Bias() const { return bias; }
			void AdjustBias(const Eigen::VectorXd& signal);
			const Connections& GetConnections() const { return connections; }
			const Connections& GetReverseConnections() const { return reverse_connections; }
			const Connection::Base& operator[](unsigned index) const { return *connections[index]; }
			Connection::Base& operator[](unsigned index) { return *connections[index]; }
			void Reset(double mean=0, double sigma=0);
			const Function& GetFunction() const { return *function.get(); }
			const Function& ChangeFunction(const Function& function);
			virtual bool IsInput() const = 0;
			virtual bool IsOutput() const = 0;
		private:
			size_t units;
			Connections connections;
			Connections reverse_connections;
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
			virtual bool IsInput() const { return true; }
			virtual bool IsOutput() const { return false; }
		};

		class Output : public Base
		{
		public:
			Output() = default;
			Output(size_t units, const Function& function);
			virtual bool IsInput() const { return false; }
			virtual bool IsOutput() const { return true; }
		};

		class Visible : public Base
		{
		public:
			Visible() = default;
			Visible(size_t units, const Function& function);
			virtual bool IsInput() const { return true; }
			virtual bool IsOutput() const { return true; }
		};

		class Hidden : public Base
		{
		public:
			Hidden() = default;
			Hidden(size_t units, const Function& function);
			virtual bool IsInput() const { return false; }
			virtual bool IsOutput() const { return false; }
		};
	}
}
