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
	void RBM::OutputProbability()
	{
		visible.ChangeFunction(Sigmoid());
	}

	namespace Learning
	{
		ContrastiveDivergence::ContrastiveDivergence(RBM& network, unsigned n) :
			Algorithm(network),
			n(n),
			weightRate(0.1),
			biasRate(0.1)
		{
		}
		
		void ContrastiveDivergence::Learn(const Data::Sample& sample)
		{
			State state(network);
			RBM& rbm = static_cast<RBM&>(network);
			Activity activity = state.Input(sample.inputs);
			Eigen::VectorXd visibleActivation = activity[rbm.visible];
			state.Apply(activity);
			activity = state.Step();
			Eigen::VectorXd hiddenActivation = activity[rbm.hidden];
			Eigen::MatrixXd positive = visibleActivation.transpose() * hiddenActivation;
			Eigen::VectorXd reconstructedActivation;
			for (unsigned i = 0; i < n; ++i)
			{	
				state.Apply(activity);
				activity = state.Reconstruct();
				if (i==n-1)	// TODO, this is ugly, because state is weird
					reconstructedActivation = activity[rbm.visible];
				state.Apply(activity);
				activity = state.Step();
			}
			Eigen::VectorXd hiddenFinalActivation = activity[rbm.hidden];

			Eigen::MatrixXd negative = reconstructedActivation.transpose() * hiddenFinalActivation;
			Eigen::MatrixXd learningSignal = positive - negative;
			learningSignal *= weightRate;
			rbm.visible[0].AdjustWeights(learningSignal);

			Eigen::VectorXd visibleBiasSignal = visibleActivation - reconstructedActivation;
			visibleBiasSignal *= biasRate;
			rbm.visible.AdjustBias(visibleBiasSignal);

			Eigen::VectorXd hiddenBiasSignal = hiddenActivation - hiddenFinalActivation;
			hiddenBiasSignal *= biasRate;
			rbm.hidden.AdjustBias(hiddenBiasSignal);
		}
	}
}
