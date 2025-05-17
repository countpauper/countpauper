#pragma once

#include "Logic/Object.h"
#include "Logic/Match.h"

namespace Angel::Logic
{

// Knowledge is a tree of clauses organized in namespaces
// A clause is a pair of a predicate and an expression 
// TODO: it should itself be an expression (Set?)  
class Knowledge
{
public:
	Knowledge();
    size_t Know(Predicate&& key, Expression&& value=Boolean(true));
	Set Matches(const Predicate& predicate) const;
	Object Infer(const Expression& e) const;
	bool Knows(const Expression& e) const;
	const Set& Root() const;
private:
	void AddDefaults();
	Set root;
};


}