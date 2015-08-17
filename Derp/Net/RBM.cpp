#include "stdafx.h"
#include "RBM.h"

RBM::RBM(size_t visible, size_t hidden) :
	visible(Add(visible))
{
	Add(this->visible, hidden);
}

