#pragma once
#include <Eigen/Dense>
#include <vector>
#include "Net.h"

namespace Net
{
	namespace Data
	{
		// TODO: Inputs/Outputs is std::map<Layer::Id, Eigen::VectorXd>
		class Base
		{
		protected:
			Base(Layer::Id layer, const Eigen::VectorXd& activation);
			Base() = default;
		public:
			virtual ~Base() = default;
			Layer::Id layer;
			Eigen::VectorXd activation;

			friend std::ostream& operator<< (std::ostream& stream, const Base& layer);
			friend std::istream& operator>> (std::istream& stream, Base& layer);
		};

		class Input : public Base
		{
		public:
			Input() = default;
			Input(Layer::Id layer, const Eigen::VectorXd& activation);

		};
		class Inputs : public std::vector <Input>
		{
	
		};

		class Output : public Base
		{
		public:
			Output() = default;
			Output(Layer::Id layer, const Eigen::VectorXd& activation);
			double MeanSquaredError(const Output& other) const;
		};

		class Outputs : public std::vector <Output>
		{
		public:
			double MeanSquaredError(const Outputs& other) const;
			const Output& operator[](Layer::Id id) const;
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