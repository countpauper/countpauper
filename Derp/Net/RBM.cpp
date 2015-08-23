#include "stdafx.h"
#include "RBM.h"
#include "Net.h"

namespace Net
{
	RBM::RBM(size_t visible, size_t hidden) :
		visible(Visible(visible, Stochastic())),
		hidden(Hidden(hidden, Stochastic()))
	{

		Undirected(this->visible, this->hidden );
	}

}
