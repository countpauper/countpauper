#pragma once
#include <Eigen/Dense>

namespace Net
{
	namespace Layer { class Base; }
	
	namespace Connection
	{
		typedef unsigned Id;

		class Base
		{
		public:
			Base(Layer::Base& a, const Layer::Base&  b);
			virtual ~Base();
			const Layer::Base& A() const { return a; }
			const Layer::Base& B() const { return b; }
			const Eigen::MatrixXd& GetWeights() const { return weights; }
			void Reset(double mean, double sigma);
		private:
			const Layer::Base& a;
			const Layer::Base& b;
			Eigen::MatrixXd weights;

			friend std::ostream& operator<< (std::ostream& stream, const Base& connection);
			friend std::istream& operator>> (std::istream& stream, Base& connection);
		};

		class Undirected : public Base
		{
		public:
			Undirected(Layer::Base& a, const Layer::Base&  b);
		};

		class Directed : public Base
		{
		public:
			Directed(Layer::Base& a, const Layer::Base&  b);
		};

		class Recurrent : public Base
		{
		public:
			Recurrent(Layer::Base& a, const Layer::Base&  b);
		};
	}
}
