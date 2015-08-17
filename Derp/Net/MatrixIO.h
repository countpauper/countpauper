#pragma once
#include <iostream>
#include <algorithm>
#include <string> 
#include <Eigen/Dense>

template<typename EigenScalar> 
EigenScalar Str2Scalar(std::string str)
{	
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	if (std::numeric_limits<EigenScalar>::has_infinity)
	{
		if (str == "1.#INF" || str == "+1.#INF")
			return std::numeric_limits<EigenScalar>::infinity();
		else if (str == "-1.#INF")
			return -std::numeric_limits<EigenScalar>::infinity();
	}
	if (std::numeric_limits<EigenScalar>::has_quiet_NaN && str == "1.#QNAN")
		return std::numeric_limits<EigenScalar>::quiet_NaN();
	std::stringstream strstream(str);
	EigenScalar result;
	strstream >> result;
	return result;
}

template<typename Derived>
std::istream & operator >>(std::istream & s, Eigen::DenseBase<Derived> & m)
{	// TODO: support Eigen::IOFormat, meaning parse and discard separators
	std::string coeffStr;
	for (Derived::Index i = 0; i < m.rows(); ++i)
	{
		for (Derived::Index j = 0; j < m.cols(); ++j)
		{
			s >> coeffStr;
			m.coeffRef(i, j) = Str2Scalar<Derived::Scalar>(coeffStr);
		}
	}
	return s;
}
