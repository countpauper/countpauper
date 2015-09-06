#include "RBM.h"
#include "Net.h"
#include "Function.h"
#include "Activation.h"

namespace Net
{
	RBM::RBM(size_t input, size_t output, size_t hidden, const Activation::Function& function) :
		input(Visible(input, function)),
		output(Visible(output, function)),
		hidden(Hidden(hidden, function))
	{
		Undirected(this->input, this->hidden);
		Undirected(this->hidden, this->output);
	}
	BinaryRBM::BinaryRBM(size_t input, size_t output, size_t hidden) :
		RBM(input, output, hidden, Activation::Stochastic())
	{
	}
	void BinaryRBM::SetProbabilistic()
	{
		input.ChangeFunction(Activation::Sigmoid());
		output.ChangeFunction(Activation::Sigmoid());
		hidden.ChangeFunction(Activation::Sigmoid());
	}
	void BinaryRBM::SetStochastic()
	{
		input.ChangeFunction(Activation::Stochastic());
		output.ChangeFunction(Activation::Stochastic());
		hidden.ChangeFunction(Activation::Stochastic());
	}

	Data::Output RBM::ComputeProbability(const Data::Input& signal) const
	{
		assert(dynamic_cast<const Activation::Stochastic*>(&input.GetFunction()) != nullptr);
		input.ChangeFunction(Activation::Sigmoid());
		Data::Inputs inputs;
		inputs.insert(std::make_pair(GetInputs()[0], signal));
		Data::Outputs outputs = Activation::Computation(*this, inputs)();
		input.ChangeFunction(Activation::Stochastic());
		return outputs[0];
	}

	double BinaryRBM::FreeEnergy(const Data::Sample& sample) const
	{
		https://www.cs.toronto.edu/~hinton/absps/guideTR.pdf (25)
		assert(GetInputs().size() == 1);
		assert(sample.inputs.size() == 1);
		Eigen::VectorXd inputSignal = sample.inputs.at(GetInputs()[0]).activation;
		Eigen::VectorXd x = input[0].weights *  inputSignal + hidden.bias;
		Eigen::VectorXd::Scalar ViAi = inputSignal.dot(input.bias);

		assert(GetOutputs().size() == 1);
		assert(sample.outputs.size() == 1);
		Eigen::VectorXd outputSignal = sample.outputs.at(GetOutputs()[0]).activation;
		x += output[GetOutputs()[0]].weights *  outputSignal + hidden.bias;
		ViAi += outputSignal.dot(output.bias);
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
			state.Input(sample.inputs);
			state.Clamp(sample.outputs);
			state.Propagate();
			state.Clear(rbm.input);
			state.Clear(rbm.output);
			state.Propagate();
			/*
			state.Apply(activity);
			Eigen::VectorXd visibleActivation = activity[rbm.input];

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
			assert(dynamic_cast<const Activation::Stochastic*>(&rbm.input.GetFunction())!=nullptr);
			assert(dynamic_cast<const Activation::Stochastic*>(&rbm.hidden.GetFunction()) != nullptr);

			rbm.SetProbabilistic();
			activity = state.Reconstruct();
			state.Apply(activity);
			Eigen::VectorXd reconstructedActivation = activity[rbm.input];

			activity = state.Step();
			state.Apply(activity);
			Eigen::VectorXd hiddenFinalActivation = activity[rbm.input];

			rbm.SetStochastic();

			Eigen::MatrixXd negative = hiddenFinalActivation * reconstructedActivation.transpose();
			Eigen::MatrixXd learningSignal = positive - negative;
			learningSignal *= weightRate;
			rbm.input[0].weights += learningSignal;

			Eigen::VectorXd visibleBiasSignal = visibleActivation - reconstructedActivation;
			visibleBiasSignal *= biasRate;
			rbm.input.bias += visibleBiasSignal;

			Eigen::VectorXd hiddenBiasSignal = hiddenInitialActivation - hiddenFinalActivation;
			hiddenBiasSignal *= biasRate;
			rbm.hidden.bias += hiddenBiasSignal;
			*/
		}
	}
}
