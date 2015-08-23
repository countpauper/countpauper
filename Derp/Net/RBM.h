#pragma once
#include "Network.h"

namespace Net
{
	class RBM : public Network
	{
	public:
		RBM(size_t visible, size_t hidden);
		double FreeEnergy(const Data::Input& inputs);
		Layer::Visible& visible;
		Layer::Hidden& hidden;

	};

}

