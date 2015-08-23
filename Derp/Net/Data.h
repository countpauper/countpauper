#pragma once
#include <Eigen/Dense>
#include <vector>

namespace Net
{
	namespace Layer { typedef unsigned Id; }

	namespace Data
	{
		// TODO: Inputs/Outputs is std::map<Layer::Id, Eigen::VectorXd> 
		class Input
		{
		public:
			Input(Layer::Id layer, const Eigen::VectorXd& activation);
			const Layer::Id layer;
			const Eigen::VectorXd activation;
		};
		typedef std::vector<Input> Inputs;

		class Output
		{
		public:
			Output(Layer::Id layer, const Eigen::VectorXd& activation);
			const Layer::Id layer;
			const Eigen::VectorXd activation;
		};
		typedef std::vector<Output> Outputs;

		class Sample
		{
		public:
			Sample() = default;
			Sample(const Inputs& inputs, const Outputs& outputs);
			Inputs inputs;
			Outputs outputs;
		};

		typedef std::vector<Sample> Set;
	}
}