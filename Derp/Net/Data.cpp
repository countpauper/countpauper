#include "Net.h"
#include "IO.h"

namespace Net
{
	namespace Data
	{

		Base::Base( const Eigen::VectorXd& activation) :
			activation(activation)
		{
		}
		Input::Input(const Eigen::VectorXd& activation) :
			Base(activation)
		{
		}
		Output::Output(const Eigen::VectorXd& activation) :
			Base(activation)
		{
		}

		double Output::MeanSquaredError(const Output& other) const
		{
			Eigen::VectorXd error = activation - other.activation;
			return error.dot(error);
		}
		double Outputs::MeanSquaredError(const Outputs& other) const
		{
			double totalE = 0;
			for (const auto& output : *this)
				totalE += output.second.MeanSquaredError(other.at(output.first));
			return totalE;
		}

		Sample::Sample(const Inputs& inputs, const Outputs& outputs) :
			inputs(inputs),
			outputs(outputs)
		{
		}


		const IO::version Base::version = 1;
		const IO::version Sample::version = 1;
		const IO::version Set::version = 1;

		std::ostream& operator<< (std::ostream& stream, const Base& data)
		{
			stream << data.version << IO::separator << data.activation.size() << IO::separator;
			stream << data.activation << std::endl;
			return stream;
		}
		std::istream& operator>> (std::istream& stream, Base& data)
		{
			IO::version v;
			stream >> v;
			if (v >= 1)
			{
				size_t size;
				stream >> size;
				data.activation.resize(size);
				stream >> data.activation;
			}
			return stream;
		}
		std::ostream& operator<< (std::ostream& stream, const Sample& sample)
		{
			stream << sample.version << IO::separator << sample.inputs.size() << IO::separator << sample.outputs.size() << std::endl;
			for (const auto& input : sample.inputs)
				stream << input.first << IO::separator << input.second;
			for (const auto& output : sample.outputs)
				stream << output.first << IO::separator << output.second;
			return stream;
		}

		std::istream& operator>> (std::istream& stream, Sample& sample)
		{
			IO::version v;
			stream >> v;
			if (v >= 1)
			{
				size_t inputs, outputs;
				stream >> inputs >> outputs;
				for (unsigned i = 0; i < inputs; ++i)
				{
					Layer::Id id;
					Input input;
					stream >> id >> input;
					sample.inputs.insert(std::make_pair(id, input));
				}
				for (unsigned i = 0; i < outputs; ++i)
				{
					Layer::Id id;
					Output output;
					stream >> id >> output;
					sample.outputs.insert(std::make_pair(id, output));
				}
			}
			return stream;
		}

		std::ostream& operator<< (std::ostream& stream, const Set& set)
		{
			stream << set.version << IO::separator << set.size() << std::endl;
			for (const auto& sample : set)
				stream << sample;
			return stream;
		}

		std::istream& operator>> (std::istream& stream, Set& set)
		{
			IO::version v;
			stream >> v;
			if (v >= 1)
			{
				size_t samples;
				stream >> samples;
				set.resize(samples);
				for (auto& sample : set)
					stream >> sample;
			}
			return stream;
		}

	}
}
