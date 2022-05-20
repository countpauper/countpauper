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
	bool Match(const Expression& e, const Knowledge& knowledge) const;
	bool Contains(const Object& e) const;
    bool Contains(const Clause& c) const;

	size_t Clauses() const;
private:
    Id id;
	std::vector<Clause> contents;   // TODO make a unique, sorted container for performance, but it requires operator< or hash of clauses (and they could be equal except for conditions)
};


}
}