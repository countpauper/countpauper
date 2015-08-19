#include "stdafx.h"
#include "RBM.h"
#include "Connection.h"

namespace Net
{
	RBM::RBM(size_t visible, size_t hidden)
	{
		Undirected(Input(visible, std::make_unique<Stochastic>()), Hidden(hidden, std::make_unique<Stochastic>()));
	}

}
