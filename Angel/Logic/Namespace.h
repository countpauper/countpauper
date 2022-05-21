#pragma once
#include <map>
#include "Id.h"
#include "Clause.h"

namespace Angel
{
namespace Logic
{

class Clause;

// A namespace is a collection of Knowledge Objects 
class Namespace
{
public:
	explicit Namespace(const Id& id);
	void Add(Clause&& e);
	Object Match(const Expression& e) const;
	bool Contains(const Expression& e) const;
	size_t Clauses() const;
private:
    Id id;
	std::vector<Clause> contents;   // TODO make a unique, sorted container for performance, but it requires operator< or hash of clauses (and they could be equal except for conditions)
};


}
}