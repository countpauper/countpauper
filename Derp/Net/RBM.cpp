#include "stdafx.h"
#include "RBM.h"
#include "Net.h"

namespace Net
{
	RBM::RBM(size_t visible, size_t hidden) :
		visible(Visible(visible, Stochastic())),
		hidden(Hidden(hidden, Stochastic()))
	{
		Undirected(this->visible, this->hidden);
	}

	namespace Learning
	{
		ContrastiveDivergence::ContrastiveDivergence(RBM& network, unsigned n) :
			Algorithm(network),
			n(n)
		{
		}
		
		void ContrastiveDivergence::Learn(const Data::Sample& sample)
		{
			State state(network);
			RBM& rbm = static_cast<RBM&>(network);
			Activity activity = state.Input(sample.inputs);
			for (unsigned i = 0; i < n; ++i)
			{	
				Eigen::VectorXd visibleActivation = activity(rbm.visible);
				activity = state.Step(); 
				Eigen::VectorXd hiddenActivation = activity(rbm.hidden);
				Eigen::MatrixXd positiveStatistics = visibleActivation * hiddenActivation;

			}
		}

	}
}
