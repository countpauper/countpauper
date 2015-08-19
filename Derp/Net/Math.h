#pragma once

#include <Eigen/Dense>

namespace Net
{
	double normalized_noise(double mean);
	double normal_noise(double mean, double sigma);
	double sigmoid(double e);
	bool p(double chance);
}
