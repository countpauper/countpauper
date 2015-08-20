#pragma once

#include <iostream>

namespace Net
{
	namespace Layer{ class Base; }
	namespace Connection { class Base; }
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

	std::ostream& operator<< (std::ostream& stream, const Network& network);
	std::istream& operator>> (std::istream& stream, Network& network);
}
