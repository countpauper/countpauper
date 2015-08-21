#pragma once
#include <vector>

namespace Net
{
	class Network;
	class RBM;

	namespace Data { class Sample; typedef std::vector<Sample> Set;  }

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

		class ContrastiveDivergence : public Algorithm
		{
		public:
			ContrastiveDivergence(RBM& network, unsigned n = 1);
			virtual void Learn(const Data::Sample& samples);
		protected:
			unsigned n;
		};


	}
}