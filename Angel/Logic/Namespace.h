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
	bool Match(const Element& e) const;
	bool Contains(const Element& e) const;
	size_t Clauses() const;
private:
    Id id;
	std::vector<Element> contents;
};


}
}