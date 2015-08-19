#include "stdafx.h"
#include "RBM.h"
#include "Connection.h"

namespace Net
{
	RBM::RBM(size_t visible, size_t hidden)
	{
		Undirected(Visible(visible, Stochastic()), Hidden(hidden, Stochastic()));
	}

}
