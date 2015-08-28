#pragma once
#include <iostream>
#include "Net.h"

namespace Net
{
	class Network;

	namespace Layer
	{
		std::ostream& operator<< (std::ostream& stream, const Base& layer);
		std::istream& operator>> (std::istream& stream, Base& layer);
	}

	namespace Connection
	{
		std::ostream& operator<< (std::ostream& stream, const Base& connection);
		std::istream& operator>> (std::istream& stream, Base& connection);
	}

	namespace Data
	{
		std::ostream& operator<< (std::ostream& stream, const Base& data);
		std::istream& operator>> (std::istream& stream, Base& data);
		std::ostream& operator<< (std::ostream& stream, const Sample& sample);
		std::istream& operator>> (std::istream& stream, Sample& sample);
		std::ostream& operator<< (std::ostream& stream, const Set& sample);
		std::istream& operator>> (std::istream& stream, Set& sample);
	}
	std::ostream& operator<< (std::ostream& stream, const Network& network);
	std::istream& operator>> (std::istream& stream, Network& network);
}
