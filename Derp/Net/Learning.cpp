#include <algorithm>
#include <random>
#include "Learning.h"
#include "Data.h"
#include "RBM.h"
#include "Activation.h"

namespace Net
{
	namespace Learning
	{

		Epochs::Epochs(unsigned n) :
			n(n)
		{
		}

		bool Epochs::Done(unsigned epoch) const 
		{ 
			return epoch >= n; 
		}

		Once::Once() : Epochs(1)
		{
		}
		Algorithm::Algorithm(Network& network) :
			network(network)
		{
		}

		void Algorithm::Train(Data::Set& trainingSet, StopCondition& stop)
		{
			// TODO: 
			//	 other & multiple stop condition
			//	different strategies with a target test Subset score
			//	and an overfitting Subsset score
			unsigned epoch = 0;
			while (!stop(epoch))
			{
				std::random_shuffle(trainingSet.begin(), trainingSet.end());
				for (const auto& sample : trainingSet)
				{
					Learn(sample);
				}
				++epoch;
			}
		}
	}
}