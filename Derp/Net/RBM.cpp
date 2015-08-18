#include "stdafx.h"
#include "RBM.h"

RBM::RBM(size_t visible, size_t hidden) :
	visible(Add(visible, std::make_unique<Stochastic>()))
{
	Add(this->visible, hidden, std::make_unique<Stochastic>());
}

