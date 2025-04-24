#pragma once
#include "Expression.h"

namespace Angel::Logic
{

// A variable is a sub category of Expression that can be assigned a value 
// on the first match as a substitution and subsequently provide that value 
// when matched a second time 
class Variable : public Expression
{
public:
    explicit Variable(const std::string_view name);
    explicit virtual operator bool() const override;
    bool operator==(const Variable& id) const;
    bool operator==(const Expression& value) const override;
    std::size_t Hash() const override;
    Object Cast(const std::type_info& t, const Knowledge& k) const override;

    Object Copy() const override;
    Match Matching(const Expression& other, const Variables& substitutions) const override;
    const Object* Condition() const override;
    Object Infer(const Knowledge& known, const Variables& substitutions) const;
private: 
    friend std::ostream& operator<<(std::ostream& os, const Variable& var);
    std::string name;
};


std::ostream& operator<<(std::ostream& os, const Variable& id);
Object var(const std::string_view name);

}
