#pragma once

#include "Network.h"
#include <iostream>


std::ostream& operator<< (std::ostream& stream, const Net::Layer& layer);
std::istream& operator>> (std::istream& stream, Net::Layer& layer);
std::ostream& operator<< (std::ostream& stream, const Net::Connection& connection);
std::istream& operator>> (std::istream& stream, Net::Connection& connection);
std::ostream& operator<< (std::ostream& stream, const Net::Network& network);
std::istream& operator>> (std::istream& stream, Net::Network& network);
