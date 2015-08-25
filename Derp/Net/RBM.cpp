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
	void RBM::SetProbabilistic()
	{
		visible.ChangeFunction(Sigmoid());
		hidden.ChangeFunction(Sigmoid());
	}
	void RBM::SetStochastic()
	{
		visible.ChangeFunction(Stochastic());
		hidden.ChangeFunction(Stochastic());
	}

	Data::Output RBM::ComputeProbability(const Data::Input& input) const
	{
		assert(dynamic_cast<const Stochastic*>(&visible.GetFunction())!=nullptr);
		visible.ChangeFunction(Sigmoid());
		Data::Inputs inputs;
		inputs.push_back(input);
		Data::Outputs outputs = Computation(*this, inputs)();
		visible.ChangeFunction(Stochastic());
		return outputs[0];
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

			// first step: activate hidden with data and compute positive correlation
			Activity activity = state.Input(sample.inputs);
			state.Apply(activity);
			Eigen::VectorXd visibleActivation = activity[rbm.visible];

			activity = state.Step();
			state.Apply(activity);
			Eigen::VectorXd hiddenInitialActivation = activity[rbm.hidden];
			Eigen::MatrixXd positive = hiddenInitialActivation * visibleActivation.transpose();

			// Gibs sampling for CDn, n>1
			for (unsigned i = 0; i < n-1; ++i)
			{	
				activity = state.Reconstruct();
				state.Apply(activity);
				activity = state.Step();
				state.Apply(activity);
			}
			// Last step, hidden and output can both use probability
			assert(dynamic_cast<const Stochastic*>(&rbm.visible.GetFunction())!=nullptr);
			assert(dynamic_cast<const Stochastic*>(&rbm.hidden.GetFunction())!=nullptr);

			rbm.SetProbabilistic();
			activity = state.Reconstruct();
			state.Apply(activity);
			Eigen::VectorXd reconstructedActivation = activity[rbm.visible];

			activity = state.Step();
			state.Apply(activity);
			Eigen::VectorXd hiddenFinalActivation = activity[rbm.hidden];

			rbm.SetStochastic();

			Eigen::MatrixXd negative = hiddenFinalActivation * reconstructedActivation.transpose();
			Eigen::MatrixXd learningSignal = positive - negative;
			learningSignal *= weightRate;
			rbm.visible[0].AdjustWeights(learningSignal);

			Eigen::VectorXd visibleBiasSignal = visibleActivation - reconstructedActivation;
			visibleBiasSignal *= biasRate;
			rbm.visible.AdjustBias(visibleBiasSignal);

			Eigen::VectorXd hiddenBiasSignal = hiddenInitialActivation - hiddenFinalActivation;
			hiddenBiasSignal *= biasRate;
			rbm.hidden.AdjustBias(hiddenBiasSignal);
		}
	}
}
