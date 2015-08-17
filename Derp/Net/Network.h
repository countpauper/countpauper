#include <Eigen/Dense>
#include <vector>
#include <memory>

class Connection;

class Layer
{
public:
	Layer(size_t units);
	virtual ~Layer();
	void Connect(Connection& connection);
	size_t Size() const;
private:
	size_t units;
	std::vector<Connection*> connections;
	Eigen::VectorXd bias;
};

class InputLayer : public Layer
{
public:
	InputLayer(size_t units);
};

class HiddenLayer : public Layer
{
public:
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
	Connection(const Layer& a, const Layer&  b);
	const Layer& B() const { return b; }
private:
	const Layer& a;
	const Layer& b;
	Eigen::MatrixXd weights;
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
protected:
	InputLayer& Add(size_t units);
	HiddenLayer& Add(Layer& layer, size_t units);
private:
	Layers layers;
	Connections connections;
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
