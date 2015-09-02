#pragma once
#include "Network.h"
#include "Learning.h"

namespace Net
{
	class RBM : public Network
	{
	public:
		RBM(size_t visible, size_t hidden, const Activation::Function& function);
		Data::Output ComputeProbability(const Data::Input& input) const;
		Layer::Visible & visible;
		Layer::Hidden& hidden;
	};
	class BinaryRBM : public RBM
	{
	public:
		BinaryRBM(size_t visible, size_t hidden);
		void SetProbabilistic();
		void SetStochastic();
		double FreeEnergy(const Data::Input& inputs) const;
	};
	namespace Learning
	{
		class ContrastiveDivergence : public Algorithm
		{
		public:
			ContrastiveDivergence(BinaryRBM& network, unsigned n = 1);
			virtual void Learn(const Data::Sample& samples);
			void Reset(const Data::Set& data, double weightSigma = 0.1) const; // TODO: see  https://www.cs.toronto.edu/~hinton/absps/guideTR.pdf chapter 8: biases based on apriori data distribution
		protected:
			unsigned n;
			double weightRate;
			double biasRate;
		};

		// TODO: PersistentContrastiveDivergence, see https://www.cs.toronto.edu/~hinton/absps/guideTR.pdf chapter 14
	}
}

