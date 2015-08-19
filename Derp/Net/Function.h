#include <Eigen/Dense>
namespace Net
{

	class Function
	{
	public:
		virtual ~Function() {}
		Eigen::VectorXd operator()(const Eigen::VectorXd& excitation) const { return Activate(excitation); }
		virtual Function* Copy() const = 0;
	private:
		virtual Eigen::VectorXd Activate(const Eigen::VectorXd& excitation) const = 0;
	};

	class Boolean : public Function
	{
	public:
		virtual Eigen::VectorXd Activate(const Eigen::VectorXd& excitation) const;
		virtual Function* Copy() const;
	};

	class Stochastic : public Function
	{
	public:
		virtual Eigen::VectorXd Activate(const Eigen::VectorXd& excitation) const;
		virtual Function* Copy() const;
	};

	class Linear : public Function
	{
	public:
		virtual Eigen::VectorXd Activate(const Eigen::VectorXd& excitation) const;
		virtual Function* Copy() const;
	};

	class Sigmoid : public Function
	{
	public:
		virtual Eigen::VectorXd Activate(const Eigen::VectorXd& excitation) const;
		virtual Function* Copy() const;
	};
}
