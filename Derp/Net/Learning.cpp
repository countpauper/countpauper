#include "stdafx.h"
#include "Learning.h"
#include "Data.h"
#include "RBM.h"
#include "Activation.h"

namespace Net
{
	namespace Learning
	{
		Algorithm::Algorithm(Network& network) :
			network(network)
		{
		}

		void Algorithm::Train(const Data::Set& samples)
		{
			// TODO: randomly until stop condition
			//	different strategies with a target test Subset score
			//	and an overfitting Subsset score
			for (const auto& sample : samples)
			{
				Learn(sample);
			}
		}
	}
}