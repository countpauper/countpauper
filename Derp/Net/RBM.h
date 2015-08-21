#pragma once
#include "Network.h"
#include "Learning.h"

namespace Net
{
	class RBM : public Network
	{
	public:
		RBM(size_t visible, size_t hidden);
		double FreeEnergy(const Data::Input& inputs) const;
		Layer::Visible & visible;
		Layer::Hidden& hidden;
	};
	namespace Learning
	{
		class ContrastiveDivergence : public Algorithm
		{
		public:
			ContrastiveDivergence(RBM& network, unsigned n = 1);
			virtual void Learn(const Data::Sample& samples);
			void Reset(const Data::Set& data, double weightSigma = 0.1) const; // TODO: see  https://www.cs.toronto.edu/~hinton/absps/guideTR.pdf chapter 8: biases based on apriori data distribution
		protected:
			unsigned n;
		};

		// TODO: PersistentContrastiveDivergence, see https://www.cs.toronto.edu/~hinton/absps/guideTR.pdf chapter 14
	}
}

