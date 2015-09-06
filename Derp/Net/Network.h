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

		class const_iterator
		{
		public:
			const_iterator(Layers::const_iterator it) : it(it) {}
			bool operator==(const const_iterator& other) { return it == other.it; }
			bool operator!=(const const_iterator& other) { return !operator==(other); }
			const_iterator& operator++() { ++it; return *this; }
			const_iterator operator++(int) { const_iterator old(*this); ++it; return old; }
			const Layer::Base& operator*() { return *it->get(); }
		private:
			Layers::const_iterator it;
		};
		const_iterator begin() const { return const_iterator(layers.cbegin()); }
		const_iterator end() const { return const_iterator(layers.cend()); }
		size_t size() const;
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
