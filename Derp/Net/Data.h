#pragma once
#include <Eigen/Dense>
#include <map>
#include "Net.h"
#include "Activation.h"

namespace Net
{
	namespace Data
	{
		class Base : public Activation::Activation
		{
		protected:
			Base(const Eigen::VectorXd& activation);
			Base() = default;
		public:
			virtual ~Base() = default;
		private:
			static const IO::version version;
			friend std::ostream& operator<< (std::ostream& stream, const Base& layer);
			friend std::istream& operator>> (std::istream& stream, Base& layer);
		};

		class Input : public Base
		{
		public:
			Input() = default;
			Input(const Eigen::VectorXd& activation);
		};
		class Inputs : public std::map<Layer::Id, Input>
		{
	
		};

		class Output : public Base
		{
		public:
			Output() = default;
			Output(const Eigen::VectorXd& activation);
			double MeanSquaredError(const Output& other) const;
		};

		class Outputs : public std::map<Layer::Id, Output>
		{
		public:
			double MeanSquaredError(const Outputs& other) const;
		};

		class Sample
		{
		public:
			Sample() = default;
			Sample(const Inputs& inputs, const Outputs& outputs);
			Inputs inputs;
			Outputs outputs;

		private:
			static const IO::version version;
			friend std::ostream& operator<< (std::ostream& stream, const Sample& layer);
			friend std::istream& operator>> (std::istream& stream, Sample& layer);
		};

		class Set : public std::vector<Sample>
		{
		public:
		private:
			static const IO::version version;
			friend std::ostream& operator<< (std::ostream& stream, const Set& layer);
			friend std::istream& operator>> (std::istream& stream, Set& layer);
		};

		std::ostream& operator<< (std::ostream& stream, const Base& data);
		std::istream& operator>> (std::istream& stream, Base& data);
		std::ostream& operator<< (std::ostream& stream, const Sample& sample);
		std::istream& operator>> (std::istream& stream, Sample& sample);
		std::ostream& operator<< (std::ostream& stream, const Set& sample);
		std::istream& operator>> (std::istream& stream, Set& sample);

	}
}