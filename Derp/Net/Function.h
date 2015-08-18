#include <Eigen/Dense>

class Function
{
public:
	virtual ~Function() {}
	virtual Eigen::VectorXd Activate(const Eigen::VectorXd& excitation) = 0;
};

class Boolean : public Function
{
public:
	virtual Eigen::VectorXd Activate(const Eigen::VectorXd& excitation);
};
class Stochastic : public Function
{
public:
	virtual Eigen::VectorXd Activate(const Eigen::VectorXd& excitation);
};

class Linear : public Function
{
public:
	virtual Eigen::VectorXd Activate(const Eigen::VectorXd& excitation);
};

class Sigmoid : public Function
{
public:
	virtual Eigen::VectorXd Activate(const Eigen::VectorXd& excitation);
};