#pragma once
#include <vector>
#include "Net.h"

namespace Net
{
	namespace Learning
	{
		class Algorithm
		{
		public:
			Algorithm(Network& network);
			void Train(const Data::Set& samples);
			virtual void Learn(const Data::Sample& sample)=0;
		protected:
			Network& network;
		};

	}
}