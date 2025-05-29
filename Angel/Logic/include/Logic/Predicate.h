#pragma once
#include "Logic/Id.h"
#include "Logic/List.h"
#include "Logic/Substitution.h"
#include "Logic/Element.h"

namespace Angel::Logic
{

class Expression;

class Predicate
{
public:
    Predicate() : id("") {}
    Predicate(const Predicate& o) = default;
    Predicate(Predicate&& o);
    Predicate& operator=(const Predicate& o);
    explicit Predicate(const Id& id, List&& arguments={});
    explicit Predicate(const std::string& name , List&& arguments={});
    
    explicit operator bool() const;
    bool operator==(const Predicate& other) const;
    std::size_t Hash() const;
    Expression Simplify() const;
    Expression Matches(const Expression& expression, const Substitutions& substitions) const;
    Predicate Substitute(const Substitutions& substitutions) const;
    Expression Infer(const class Knowledge& knowledge, const Substitutions& substitutions) const;
private:
    friend std::ostream& operator<<(std::ostream& os, const Predicate& );
    Id id;
	List arguments;
};


std::ostream& operator<<(std::ostream& os, const Predicate& );


}