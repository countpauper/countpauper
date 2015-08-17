#include <Eigen/Dense>
#include <vector>
#include <memory>

class Connection;

class Layer
{
public:
	typedef std::vector<Connection*> Connections;
	Layer();
	Layer(size_t units);
	virtual ~Layer();
	void Connect(Connection& connection);
	size_t Size() const;
	const Eigen::VectorXd& Bias() const { return bias; }
	const Connections& GetConnections() const { return connections;  }
private:
	size_t units;
	Connections connections;
	Eigen::VectorXd bias;

	friend std::ostream& operator<< (std::ostream& stream, const Layer& layer);
	friend std::istream& operator>> (std::istream& stream, Layer& layer);
};

class InputLayer : public Layer
{
public:
	InputLayer();
	InputLayer(size_t units);
};

class HiddenLayer : public Layer
{
public:
	HiddenLayer();
	HiddenLayer(size_t units);

};

class LayerState
{
public:
	LayerState(const Layer& layer);
	void Activate(const Eigen::VectorXd& activation);
	const Layer& GetLayer() const { return layer;  }
private:
	const Layer& layer;
	Eigen::VectorXd activation;
};

class Connection
{
public:
	Connection(Layer& a, const Layer&  b);
	const Layer& A() const { return a; }
	const Layer& B() const { return b; }
	const Eigen::MatrixXd& Weights() const { return weights; }
private:
	const Layer& a;
	const Layer& b;
	Eigen::MatrixXd weights;

	friend std::ostream& operator<< (std::ostream& stream, const Connection& connection);
	friend std::istream& operator>> (std::istream& stream, Connection& connection);
};

class NetworkState;
class Sample;

class Network
{
public:
	typedef std::vector<std::unique_ptr<Layer>> Layers;
	typedef std::vector<std::unique_ptr<Connection>> Connections;
	const Layers& GetLayers() const;
	NetworkState Activate(const Sample& sample);
	InputLayer& Add(size_t units);
	HiddenLayer& Add(Layer& layer, size_t units);
private:
	Layers layers;
	Connections connections;

	 friend std::ostream& operator<< (std::ostream& stream, const Network& network);
	 friend std::istream& operator>> (std::istream& stream, Network& network);
};

class Sample
{
public:
	Sample(const Eigen::VectorXd& activation);
	Eigen::VectorXd Activation(size_t from, size_t count) const;
protected:
	Eigen::VectorXd activation;
};


class NetworkState
{
public:
	typedef std::vector<LayerState> Layers;

	NetworkState(const Network& network);
	void Activate(const Sample& sample);
private:
	Layers inputLayers;
};
