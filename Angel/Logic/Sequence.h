#pragma once
#include <vector>
#include "Id.h"
#include "Element.h"

namespace Angel
{
namespace Logic
{

class Sequence : public Value
{
public:
	Sequence();
	bool operator==(const Value& other) const override;
private:
	std::vector<Element> contents;
};

Element sequence();

}
}
