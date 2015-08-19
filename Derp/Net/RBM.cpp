#include "stdafx.h"
#include "RBM.h"

namespace Net
{
	RBM::RBM(size_t visible, size_t hidden) :
		visible(Add(visible, Stochastic()))
	{
		Add(this->visible, hidden, Stochastic());
	}

}
