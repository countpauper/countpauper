#pragma once

#include <Eigen/Dense>
#include <vector>
#include <memory>

#include "Activation.h"
#include "Function.h"

namespace Net
{
	namespace Layer { class Base; class Input; class Output; class Hidden; class Visible; }
	namespace Connection {class Base;	class Directed;  class Undirected; class Recurrent; }
	class Network;
	class Sample;


	class Network
	{
	public:
		typedef std::vector<std::unique_ptr<Layer::Base>> Layers;
		typedef std::vector<std::unique_ptr<Connection::Base>> Connections;

		const Layers& GetLayers() const;
		const Layer::Base& operator[](unsigned index) const { return *layers[index].get(); }
		Layer::Base& operator[](unsigned index) { return *layers[index].get(); }
		Eigen::VectorXd Compute(const Sample& sample);
		Layer::Input& Input(size_t units);
		Layer::Output& Output(size_t units, const Function& function);
		Layer::Visible& Visible(size_t units, const Function& function);
		Layer::Hidden& Hidden(size_t units, const Function& function);
		Connection::Directed& Directed(Layer::Base& a, Layer::Base &b);
		Connection::Undirected& Undirected(Layer::Base& a, Layer::Base &b);
		void Reset(double mean = 0, double sigma = 0);
	private:
		Layers layers;
		Connections connections;

		friend std::ostream& operator<< (std::ostream& stream, const Network& network);
		friend std::istream& operator>> (std::istream& stream, Network& network);
	};


}
