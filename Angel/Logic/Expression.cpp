#include "Logic/Expression.h"
#include "Logic/Object.h"
#include "Logic/Boolean.h"
#include "Logic/Integer.h"
#include "Logic/Id.h"
#include "Logic/Predicate.h"
#include "Logic/Clause.h"
#include "Logic/Sequence.h"
#include "Logic/Set.h"
#include "Logic/Conjunction.h"
#include "Logic/Disjunction.h"

#include <sstream>

namespace Angel::Logic
{


Expression::operator std::string() const
{
    std::stringstream s;
    //s << *this;
    return s.str();
}

std::ostream& operator<<(std::ostream& os, const Expression& e)
{
    // TODO variant and std::visit, although the resulting 
    // instance might be relatively large. 
    // In that case the base expression can go
    if (const auto* b = dynamic_cast<const Angel::Logic::Boolean*>(&e))
    {
        os << *b;
    }
    else if (const auto* i = dynamic_cast<const Angel::Logic::Integer*>(&e))
    {
        os << *i;
    }
    else if (const auto* id = dynamic_cast<const Angel::Logic::Id*>(&e))
    {
        os << *id;
    }
    else if (const auto* p = dynamic_cast<const Angel::Logic::Predicate*>(&e))
    {
        os << *p;
    }
    else if (const auto* c = dynamic_cast<const Angel::Logic::Clause*>(&e))
    {
        os << *c;
    }
    else if (const auto* seq = dynamic_cast<const Angel::Logic::Sequence*>(&e))
    {
        os << *seq;
    }
    else if (const auto* set = dynamic_cast<const Angel::Logic::Set*>(&e))
    {
        os << *set;
    }
    else if (const auto* con = dynamic_cast<const Angel::Logic::Conjunction*>(&e))
    {
        os << *con;
    }
    else if (const auto* dis = dynamic_cast<const Angel::Logic::Disjunction*>(&e))
    {
        os << *dis;
    }
    else 
    {
        assert(false && "Expression type not implemented");
    }
    return os;
}

}

