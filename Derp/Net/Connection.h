#pragma once
#include <Eigen/Dense>
#include "Net.h"

namespace Net
{

	namespace Connection
	{
		typedef unsigned Id;

		class Base
		{
		protected:
			Base(const Layer::Base& a, const Layer::Base&  b);
		public:
			virtual ~Base();
			const Layer::Base& A() const { return a; }
			const Layer::Base& B() const { return b; }
			const Eigen::MatrixXd& GetWeights() const { return weights; }
			void AdjustWeights(const Eigen::MatrixXd& delta);
			void Reset(double mean = 0, double sigma = 0);
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
			Undirected(Layer::Base& a, Layer::Base&  b);
		};

		class Directed : public Base
		{
		public:
			Directed(Layer::Base& a, const Layer::Base&  b);
		};

		class Recurrent : public Base
		{
		public:
			Recurrent(const Layer::Base& a, Layer::Base&  b);
		};
	}
}
