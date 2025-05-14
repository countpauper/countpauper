#pragma once
#include "Logic/Match.h"
#include <string>

namespace Angel::Logic
{

// A variable is a sub category of Expression that can be assigned a value 
// on the first match as a substitution and subsequently provide that value 
// when matched again
class Variable 
{
public:
    explicit Variable(const std::string_view name);
    explicit virtual operator bool() const;
    bool operator==(const Variable& id) const;
    std::size_t Hash() const;
    Match Matches(const Object& other, const Variables& variables) const;
    Object Compute(const class Knowledge& k, const Variables& substitutions) const;

//    Object Cast(const std::type_info& t, const Knowledge& k) const override;

//    Object Copy() const override;
//    Match Matching(const Expression& other, const Variables& substitutions) const override;
//    const Object* Condition() const override;
//    Object Infer(const Knowledge& known, const Variables& substitutions) const;
private: 
    friend std::ostream& operator<<(std::ostream& os, const Variable& var);
    std::string name;
};


std::ostream& operator<<(std::ostream& os, const Variable& id);

}
