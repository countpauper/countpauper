#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

namespace Net
{
	double normalized_noise(double mean)
	{
		static boost::mt19937 rng;
		static boost::normal_distribution<> nd(mean, 1.0);
		return nd(rng);
	}

	double normal_noise(double mean, double sigma)
	{
		static boost::mt19937 rng;
		static boost::normal_distribution<> nd(mean, sigma);
		return nd(rng);
	}

	double sigmoid(double e)
	{
		return 1.0 / (1.0 + exp(-e));
	}

	bool p(double chance)
	{
		double rnd = double(rand()) / double(RAND_MAX);
		return chance > rnd;
	}

}
