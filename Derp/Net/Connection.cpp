#include "stdafx.h"
#include "Connection.h"
#include "Network.h"
#include "Layer.h"
#include "Math.h"

namespace Net
{	
	namespace Connection
	{
		Base::Base(Layer::Base& a, const Layer::Base& b) :
			a(a),
			b(b),
			weights(b.Size(), a.Size())
		{
			a.Connect(*this);
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

		Undirected::Undirected(Layer::Base& a, const Layer::Base&  b) : Base(a, b)
		{
		}

		Directed::Directed(Layer::Base& a, const Layer::Base&  b) : Base(a, b)
		{
		}

		Recurrent::Recurrent(Layer::Base& a, const Layer::Base&  b) : Base(a, b)
		{
		}
	}
}
