#pragma once
#include <Eigen/Dense>
#include <vector>
#include <memory>
#include "Net.h"


namespace Net
{
	namespace Layer
	{
		typedef unsigned Id;

		class Base
		{
		public:
			typedef std::vector<std::reference_wrapper<Connection::Base>> Connections;
			Base();
			Base(size_t units, const Activation::Function& function);
			Base(const Base& other);
			virtual ~Base();
			void Connect(Connection::Base& connection);
			size_t Size() const;
			const Connection::Base& operator[](unsigned index) const;
			Connection::Base& operator[](unsigned index);
			void Reset(double mean=0, double sigma=0);
			const Activation::Function& GetFunction() const { return *function.get(); }
			const Activation::Function& ChangeFunction(const Activation::Function& function);
			virtual bool IsInput() const = 0;
			virtual bool IsOutput() const = 0;
		public:
			Eigen::VectorXd bias;
			Connections connections;
			Connections reverse_connections;
		private:
			std::unique_ptr<Activation::Function> function;

			static const IO::version version;
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
			Output(size_t units, const Activation::Function& function);
			virtual bool IsInput() const { return false; }
			virtual bool IsOutput() const { return true; }
		};

		class Visible : public Base
		{
		public:
			Visible() = default;
			Visible(size_t units, const Activation::Function& function);
			virtual bool IsInput() const { return true; }
			virtual bool IsOutput() const { return true; }
		};

		class Hidden : public Base
		{
		public:
			Hidden() = default;
			Hidden(size_t units, const Activation::Function& function);
			virtual bool IsInput() const { return false; }
			virtual bool IsOutput() const { return false; }
		};

		std::ostream& operator<< (std::ostream& stream, const Base& layer);
		std::istream& operator>> (std::istream& stream, Base& layer);
	}
}
