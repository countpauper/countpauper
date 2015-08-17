#pragma once

#include "Network.h"
#include <iostream>

std::ostream& operator<< (std::ostream& stream, const Layer& layer);
std::istream& operator>> (std::istream& stream, Layer& layer);
std::ostream& operator<< (std::ostream& stream, const Connection& connection);
std::istream& operator>> (std::istream& stream, Connection& connection);
std::ostream& operator<< (std::ostream& stream, const Network& network);
std::istream& operator>> (std::istream& stream, Network& network);