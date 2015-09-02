#pragma once
#include <vector>
#include "Net.h"

namespace Net
{
	namespace Learning
	{
		class StopCondition
		{
		public:
			bool operator()(unsigned epoch) const { return Done(epoch); }
			virtual bool Done(unsigned epoch) const = 0;
		};
		class Epochs : public StopCondition
		{
		public:
			Epochs(unsigned n);
			unsigned n;
			virtual bool Done(unsigned epoch) const;

		};

		class Once : public Epochs
		{
		public:
			Once();
		};
		class Algorithm
		{
		public:
			Algorithm(Network& network);
			void Train(Data::Set& samples, StopCondition& stop=Once());
			virtual void Learn(const Data::Sample& sample)=0;
		protected:
			Network& network;
		};

	}
}