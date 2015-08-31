#pragma once
#include <Eigen/Dense>
#include <map>
#include "Net.h"

namespace Net
{
	namespace Data
	{
		class Base
		{
		protected:
			Base(const Eigen::VectorXd& activation);
			Base() = default;
		public:
			virtual ~Base() = default;
			Eigen::VectorXd activation;

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

			friend std::ostream& operator<< (std::ostream& stream, const Sample& layer);
			friend std::istream& operator>> (std::istream& stream, Sample& layer);
		};

		class Set : public std::vector<Sample>
		{
		public:
		};
	}
}