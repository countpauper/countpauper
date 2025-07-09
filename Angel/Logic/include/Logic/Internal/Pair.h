#pragma once
#include "Logic/Hypothesis.h"
#include <memory>

namespace Angel::Logic
{

class Expression;

class Pair 
{ 
public:
    template<class TL, class TR>
    Pair(TL&& lhs, TR&& rhs) :
        lhs(std::make_unique<Expression>(std::forward<TL>(lhs))),
        rhs(std::make_unique<Expression>(std::forward<TR>(rhs)))
    {  
    }
    explicit operator bool() const;
    std::size_t size() const;
    Set Assumptions() const;
    const Expression& Left() const;
    Expression& Left();
    const Expression& Right() const;
    Expression& Right();
    std::size_t Hash() const;
protected:
    Pair(const Pair& o);
    Pair(Pair&& o); 
    Pair& operator=(const Pair& o);
    Pair& operator=(Pair&& o);
    bool operator==(const Pair& o) const;

    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
};

}