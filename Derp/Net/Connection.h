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
			void Reset(double mean = 0, double sigma = 0);
		public:
			const Layer::Base& a;
			const Layer::Base& b;
			Eigen::MatrixXd weights;
		private:
			static const IO::version version;
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
		std::ostream& operator<< (std::ostream& stream, const Base& connection);
		std::istream& operator>> (std::istream& stream, Base& connection);
	}
}
