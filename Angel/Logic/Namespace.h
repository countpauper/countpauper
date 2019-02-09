#pragma once
#include <vector>
#include "Id.h"
#include "Element.h"

namespace Angel
{
namespace Logic
{

class Namespace
{
public:
	explicit Namespace(const Id& id);
	void Add(Element&& e);
	bool Query(const Element& e) const;
private:
    Id id;
	std::vector<Element> contents;
};


}
}