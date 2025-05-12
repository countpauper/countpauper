#pragma once

#include "Logic/Object.h"
#include "Logic/Match.h"

namespace Angel
{
namespace Logic
{

// Knowledge is a tree of clauses organized in namespaces
// A clause is a pair of a predicate and an expression 
// TODO: it should itself be an expression (Set?)  
class Knowledge
{
public:
	Knowledge();
    size_t Know(Object&& e);
	Object Query(const Object& e, const Variables& substitutions=Variables()) const;
	bool Knows(const Object& e) const;
	size_t size() const;
	bool empty() const;
private:
	Set root;
};


}
}