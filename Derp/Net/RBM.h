#pragma once
#include "Network.h"

class RBM : public Network
{
public:
	RBM(size_t visible, size_t hidden);
private:
	InputLayer& visible;
};

