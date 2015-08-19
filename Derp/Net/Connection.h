#pragma once
#include <Eigen/Dense>

namespace Net
{
	class Layer;
	
	namespace Connection
	{
		class Base
		{
		public:
			Base(Layer& a, const Layer&  b);
			virtual ~Base();
			const Layer& A() const { return a; }
			const Layer& B() const { return b; }
			const Eigen::MatrixXd& GetWeights() const { return weights; }
			void Reset(double mean, double sigma);
		private:
			const Layer& a;
			const Layer& b;
			Eigen::MatrixXd weights;

			friend std::ostream& operator<< (std::ostream& stream, const Base& connection);
			friend std::istream& operator>> (std::istream& stream, Base& connection);
		};

		class Undirected : public Base
		{
		public:
			Undirected(Layer& a, const Layer&  b);
		};

		class Directed : public Base
		{
		public:
			Directed(Layer& a, const Layer&  b);
		};

		class Recurrent : public Base
		{
		public:
			Recurrent(Layer& a, const Layer&  b);
		};
	}
}
