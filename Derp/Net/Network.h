#pragma once

#include <Eigen/Dense>
#include <vector>
#include <memory>

#include "Net.h"

namespace Net
{
	class Network
	{
	public:
		typedef std::vector<std::unique_ptr<Layer::Base>> Layers;
		typedef std::vector<std::unique_ptr<Connection::Base>> Connections;
		typedef std::vector<Layer::Id> LayerIds;

		// Structure
		size_t Size() const;
		const Layer::Base& operator[](Layer::Id id) const { return *layers[id].get(); }
		Layer::Base& operator[](Layer::Id id) { return *layers[id].get(); }
		LayerIds GetOutputs() const;
		LayerIds GetInputs() const;
		
		// Construction, TODO: redesign interface with some operator Network << Layer() << COnnection() << Layer()
		Layer::Input& Input(size_t units);
		Layer::Output& Output(size_t units, const Activation::Function& function);
		Layer::Visible& Visible(size_t units, const Activation::Function& function);
		Layer::Hidden& Hidden(size_t units, const Activation::Function& function);
		Connection::Directed& Directed(Layer::Base& a, Layer::Base &b);
		Connection::Undirected& Undirected(Layer::Base& a, Layer::Base &b);
		void Reset(double mean = 0, double sigma = 0);
		
		// Computation 
		Data::Outputs operator()(const Data::Inputs& inputs) const { return Compute(inputs); }
		double MeanSquaredError(const Data::Set& data) const;
	protected:
		Data::Outputs Compute(const Data::Inputs& inputs) const;
	private:
		Layers layers;
		Connections connections;

		static const IO::version version;
		friend std::ostream& operator<< (std::ostream& stream, const Network& network);
		friend std::istream& operator>> (std::istream& stream, Network& network);
	};

	std::ostream& operator<< (std::ostream& stream, const Network& network);
	std::istream& operator>> (std::istream& stream, Network& network);
}
