#pragma once
#include "Logic/Id.h"
#include "Logic/List.h"
#include "Logic/Match.h"
#include "Logic/Element.h"

namespace Angel::Logic
{

class Expression;

class Predicate
{
public:
    Predicate() : id("") {}
    explicit Predicate(const Id& id, List&& arguments={});
    explicit Predicate(const std::string& name , List&& arguments={});
    
    explicit operator bool() const;
    bool operator==(const Predicate& other) const;
    std::size_t Hash() const;
    Match Matches(const Expression& expression, const Variables& vars) const;
    Object Infer(const class Knowledge& knowledge, const Variables& substitutions={}) const;
private:
    friend std::ostream& operator<<(std::ostream& os, const Predicate& );
    Id id;
	List arguments;
};


std::ostream& operator<<(std::ostream& os, const Predicate& );


}