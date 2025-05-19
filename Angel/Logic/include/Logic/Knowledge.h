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
	Expression Matches(const Predicate& predicate) const;
	Object Infer(const Expression& e) const;
	bool Knows(const Expression& e) const;
	const Bag& Root() const;
private:
	void AddDefaults();
	// TODO: really a namespace is not a set. It should be a multimap where cat: gimmy, cat: gizmo are both defined
	// and the set can just be back to being non associative (while keeping the tests the same)
	// and the set should be able to contain associations (and support .), but for now just not optimized
	// perhaps the . operator is always the same as `in` where it gives a boolean if the value is found 
	// but gives the associated value if it is found AND a pair. That makes the @ more specifically  `any`
	// this way even lists can contain pairs. they'd just be multi-maps and `.` would return a list or a (falsey) empty
	// list if the element is not in there. Only for unique containers `.` would return a single unique value.  
	Bag root;	// TODO should be a bag as it is not ordered, but whatever 
};


}