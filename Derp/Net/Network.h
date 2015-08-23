#pragma once

#include <Eigen/Dense>
#include <vector>
#include <memory>

#include "Data.h"

namespace Net
{
	namespace Layer { class Base; class Input; class Output; class Hidden; class Visible; typedef unsigned Id;  }
	namespace Connection {class Base;	class Directed;  class Undirected; class Recurrent; }
	class Network;
	class Function;

	class Network
	{
	public:
		typedef std::vector<std::unique_ptr<Layer::Base>> Layers;
		typedef std::vector<std::unique_ptr<Connection::Base>> Connections;
		typedef std::vector<Layer::Id> LayerIds;

		const Layers& GetLayers() const;
		const Layer::Base& operator[](Layer::Id id) const { return *layers[id].get(); }
		Layer::Base& operator[](Layer::Id id) { return *layers[id].get(); }
		LayerIds GetOutputs() const;
		LayerIds GetInputs() const;

		Layer::Input& Input(size_t units);
		Layer::Output& Output(size_t units, const Function& function);
		Layer::Visible& Visible(size_t units, const Function& function);
		Layer::Hidden& Hidden(size_t units, const Function& function);
		Connection::Directed& Directed(Layer::Base& a, Layer::Base &b);
		Connection::Undirected& Undirected(Layer::Base& a, Layer::Base &b);
		void Reset(double mean = 0, double sigma = 0);
		Data::Outputs operator()(const Data::Inputs& inputs) const { return Compute(inputs); }
	protected:
		Data::Outputs Compute(const Data::Inputs& inputs) const;
	private:
		Layers layers;
		Connections connections;

		friend std::ostream& operator<< (std::ostream& stream, const Network& network);
		friend std::istream& operator>> (std::istream& stream, Network& network);
	};


}
