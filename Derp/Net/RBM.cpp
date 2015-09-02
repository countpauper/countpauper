#include "RBM.h"
#include "Net.h"
#include "Function.h"
#include "Activation.h"

namespace Net
{
	RBM::RBM(size_t visible, size_t hidden, const Activation::Function& function) :
		visible(Visible(visible, function)),
		hidden(Hidden(hidden, function))
	{
		Undirected(this->visible, this->hidden);
	}
	BinaryRBM::BinaryRBM(size_t visible, size_t hidden) :
		RBM(visible, hidden, Activation::Stochastic())
	{
	}
	void BinaryRBM::SetProbabilistic()
	{
		visible.ChangeFunction(Activation::Sigmoid());
		hidden.ChangeFunction(Activation::Sigmoid());
	}
	void BinaryRBM::SetStochastic()
	{
		visible.ChangeFunction(Activation::Stochastic());
		hidden.ChangeFunction(Activation::Stochastic());
	}

	Data::Output RBM::ComputeProbability(const Data::Input& input) const
	{
		assert(dynamic_cast<const Activation::Stochastic*>(&visible.GetFunction()) != nullptr);
		visible.ChangeFunction(Activation::Sigmoid());
		Data::Inputs inputs;
		inputs.insert(std::make_pair(GetInputs()[0], input));
		Data::Outputs outputs = Activation::Computation(*this, inputs)();
		visible.ChangeFunction(Activation::Stochastic());
		return outputs[0];
	}

	double BinaryRBM::FreeEnergy(const Data::Input& input) const
	{
//		wx_b = T.dot(v_sample, self.W) + self.hbias
//			vbias_term = T.dot(v_sample, self.vbias)
//			hidden_term = T.sum(T.log(1 + T.exp(wx_b)), axis = 1)
//			return -hidden_term - vbias_term;

		// also see https://www.cs.toronto.edu/~hinton/absps/guideTR.pdf (25)
		Eigen::VectorXd x = visible[0].weights *  input.activation + hidden.bias;
		Eigen::VectorXd::Scalar ViAi = input.activation.dot(visible.bias);
		Eigen::VectorXd hidden_term = x.unaryExpr([](const Eigen::VectorXd::Scalar& Xj) { return log(1 + exp(Xj));  });
		return -hidden_term.sum() - ViAi;
	}

	namespace Learning
	{
		ContrastiveDivergence::ContrastiveDivergence(BinaryRBM& network, unsigned n) :
			Algorithm(network),
			n(n),
			weightRate(0.1),
			biasRate(0.1)
		{
		}
		
		void ContrastiveDivergence::Learn(const Data::Sample& sample)
		{
			Activation::State state(network);
			BinaryRBM& rbm = static_cast<BinaryRBM&>(network);

			// first step: activate hidden with data and compute positive correlation
			Activation::Activity activity = state.Input(sample.inputs);
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
			assert(dynamic_cast<const Activation::Stochastic*>(&rbm.visible.GetFunction())!=nullptr);
			assert(dynamic_cast<const Activation::Stochastic*>(&rbm.hidden.GetFunction()) != nullptr);

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
			rbm.visible[0].weights += learningSignal;

			Eigen::VectorXd visibleBiasSignal = visibleActivation - reconstructedActivation;
			visibleBiasSignal *= biasRate;
			rbm.visible.bias += visibleBiasSignal;

			Eigen::VectorXd hiddenBiasSignal = hiddenInitialActivation - hiddenFinalActivation;
			hiddenBiasSignal *= biasRate;
			rbm.hidden.bias += hiddenBiasSignal;
		}
	}
}
