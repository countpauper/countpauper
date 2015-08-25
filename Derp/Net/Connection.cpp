#include "stdafx.h"
#include "Connection.h"
#include "Network.h"
#include "Layer.h"
#include "Math.h"

namespace Net
{	
	namespace Connection
	{
		Base::Base(const Layer::Base& a, const Layer::Base& b) :
			a(a),
			b(b),
			weights(b.Size(), a.Size())
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

		void Base::AdjustWeights(const Eigen::MatrixXd& delta)
		{
			weights += delta;
		}

		Undirected::Undirected(Layer::Base& a, Layer::Base&  b) : Base(a, b)
		{
			a.Connect(*this);
			b.Connect(*this);
		}

		Directed::Directed(Layer::Base& a, const Layer::Base&  b) : Base(a, b)
		{
			a.Connect(*this);
		}

		Recurrent::Recurrent(const Layer::Base& a, Layer::Base&  b) : Base(a, b)
		{
			b.Connect(*this);
		}
	}
}
