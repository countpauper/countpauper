#pragma once
#include <Eigen/Dense>
#include <map>
#include "Net.h"

namespace Net
{
namespace Activation
{
	class Activation
	{
	public:
		Activation() = default;
		Activation(const Eigen::VectorXd& activation);
		Activation& operator=(const Activation& other);
		Eigen::VectorXd activation;
	};

	class Activity : private  std::map < const Layer::Base*, Activation >
	{
	public:
		Eigen::VectorXd operator[](const Layer::Base& layer) const { return GetActivation(layer); }
		bool IsActive(const Layer::Base& layer) const;
		void Activate(const Layer::Base& layer, const Activation& activation);
		void Clear(const Layer::Base& layer);
	protected:
		Eigen::VectorXd GetActivation(const Layer::Base& layer) const;
	};
}
}