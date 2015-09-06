#include "Connection.h"
#include "Network.h"
#include "Layer.h"
#include "Math.h"
#include "IO.h"

namespace Net
{	
	namespace Connection
	{
		Base::Base(Layer::Base& a, Layer::Base& b) :
			a(a),
			b(b),
			weights(b.size(), a.size())
		{
		}

		Base::~Base()
		{
		}

		void Base::Reset(double mean, double sigma)
		{
			weights.setConstant(mean);
			if (sigma != 0)
			{
				weights = weights.binaryExpr(Eigen::MatrixXd::Constant(weights.rows(), weights.cols(), sigma), std::ptr_fun(normal_noise));
			}
		}

		Undirected::Undirected(Layer::Base& a, Layer::Base&  b) : Base(a, b)
		{
			a.Connect(*this);
			b.Connect(*this);
		}

		Directed::Directed(Layer::Base& a, Layer::Base&  b) : Base(a, b)
		{
			a.Connect(*this);
		}

		Recurrent::Recurrent(Layer::Base& a, Layer::Base&  b) : Base(a, b)
		{
			b.Connect(*this);
		}

		const IO::version Base::version = 1;

		std::ostream& operator<< (std::ostream& stream, const Base& connection)
		{
			stream << connection.version << std::endl;
			stream << connection.weights << std::endl;

			return stream;
		}

		std::istream& operator>> (std::istream& stream, Base& connection)
		{
			IO::version v;
			stream >> v;
			if (v >= 1)
			{
				connection.weights.resize(connection.a.size(), connection.b.size());
				stream >> connection.weights;
			}

			return stream;
		}
	}
}
