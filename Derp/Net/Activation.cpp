#include <Eigen/Dense>
#include <algorithm>
#include "Net.h"
#include "Activation.h"

namespace Net
{
namespace Activation
{
	Activation::Activation(const Eigen::VectorXd& activation) :
		activation(activation)
	{
	}

	Activation& Activation::operator=(const Activation& other)
	{
		activation = other.activation;
		return *this;
	}

	void Activity::Activate(const Layer::Base& layer, const Activation& activation)
	{
		if (layer.size() != activation.activation.size())
			throw std::runtime_error("Layer::Base activated with incompatible state");
		emplace(std::pair<const Layer::Base*, Activation>(&layer, activation));
	}

	bool Activity::IsActive(const Layer::Base& layer) const
	{
		return 0 < std::count_if(cbegin(), cend(), [&layer](const std::pair<const Layer::Base*, Activation>& activation){ return activation.first == &layer;  });
	}
	Eigen::VectorXd Activity::GetActivation(const Layer::Base& layer) const
	{
		const_iterator it = std::find_if(cbegin(), cend(), [&layer](const std::pair<const Layer::Base*,Activation>& activation){ return activation.first == &layer;  });
		if (it == end())
			throw std::out_of_range("No activity for that layer");
		else
			return it->second.activation;
	}

	void Activity::Clear(const Layer::Base& layer)
	{
		for (iterator it = begin(); it != end(); ++it)
		{
			if (it->first == &layer)
				it = erase(it);
		}
	}

}	
}

