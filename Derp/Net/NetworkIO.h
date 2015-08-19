#pragma once

#include "Network.h"
#include <iostream>

namespace Net
{
	std::ostream& operator<< (std::ostream& stream, const Layer& layer);
	std::istream& operator>> (std::istream& stream, Layer& layer);

	namespace Connection
	{
		std::ostream& operator<< (std::ostream& stream, const Base& connection);
		std::istream& operator>> (std::istream& stream, Base& connection);
	}

	std::ostream& operator<< (std::ostream& stream, const Network& network);
	std::istream& operator>> (std::istream& stream, Network& network);
}
