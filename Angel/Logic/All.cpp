#include "Logic/All.h"
#include "Logic/Expression.h"
#include "Logic/Internal/container_iterator.h"

namespace Angel::Logic
{
         
All::All(std::initializer_list<Expression> items) : 
    All(List(items))
{
}

All::All(const std::string_view name) : 
    Individual(name.empty()?Expression():Id(name)) 
{
}

All::All(Expression&& e) : 
    Individual(std::move(e))
{
    if (!empty() &&
        !content->Is<Id>() && 
        !content->Is<Container>() && 
        !content->Is<Variable>())
        throw std::invalid_argument("All operand must be an Id, a Container, or a Variable.");
}   

All All::Simplify() const
{
    return All(content->Simplify());
}

Expression MatchValliable(const Variable& var, const Expression& expression, const Hypothesis& hypothesis, bool reverse)
{
    auto substitute = var->Substitute(hypothesis);
    if (substitute != var)
        return substitute.Matches(expression, hypothesis);
    if (var.empty())
        return Boolean(true);   // anonymous
    if (reverse)
        return Equal{var, expression};
    else
        return Equal{expression, var};
}

Expression All::Matches(Collection_subrange range, const Hypothesis& hypothesis, bool reverse) const
{
    auto var = GetVariable();
    if (var)
    {    
        auto substitute = var->Substitute(hypothesis);
        if (substitute != *var)
        {
            if (const List* list = substitute.GetIf<List>())
            {
                return list->Collection::Matches(range, hypothesis);
            }
            else 
            {
                assert(false); // unimplemented, instead of a collection range it should just be a container range
                return Boolean(false);
            }            
        }

        if (var->empty())
            return Boolean(true);   // anonymous
        if (reverse)
            return Equal{*var, List(range)};
        else
            return Equal{List(range), *var};
    }
    else 
    {
        if (const auto* list = content->GetIf<List>())
        {
            return list->Collection::Matches(range, hypothesis);
        }
        else 
        {
            assert(false);  // also here content should anyway be a container. Generic match containers 
            return Boolean(false);
        }            

    }
}

Expression All::Matches(const Expression& expression, const Hypothesis& hypothesis, bool reverse) const
{
    auto var = GetVariable();
    if (var)
    {
        return MatchValliable(*var, expression, hypothesis, reverse);
    }
    else 
    {
        if (const All* other = expression.GetIf<All>())
        {
            return content->Matches(**other, hypothesis);
        }
        return Boolean(false);
    }
}

All All::Substitute(const Hypothesis& hypothesis) const
{
    auto var = GetVariable();
    if (var)
    {
        auto varSubstitute = var->Substitute(hypothesis);
        if (varSubstitute == var)
            return *this;
        else 
            return All(std::move(varSubstitute));
    }
    else 
        return All(content->Substitute(hypothesis));
}

Expression All::Infer(const class Knowledge& k, const Hypothesis& hypothesis, Trace& trace) const
{
    auto var = GetVariable();
    if (var)
    {
        auto substitution = var->Substitute(hypothesis);
        if (substitution==*var)
            return *this;
        else
            return All(substitution.Infer(k, hypothesis, trace));
    }
    if (empty())
        return *this;
    const auto* id = content->GetIf<Id>();
    if (id)
        return *id; // TODO tupling, get the variable id from the hypthesis 
        
    return All(content->Infer(k, hypothesis, trace));
}

bool All::operator==(const All& other) const
{
    return Individual::operator==(other);
}

std::size_t All::size() const
{
    if (content)
        return content->size();
    else
        return 0;
}

std::optional<Variable> All::GetVariable() const
{
    if (!content)
        return Variable("");
    const auto* id = content->GetIf<Id>();
    if (id)
        return Variable(*id);
    return std::optional<Variable>();
}

Set All::Assumptions() const
{
    auto v = GetVariable();
    if (v)
        return Set{*this};
    else
        return Individual::Assumptions(); 
}

const_container_iterator All::begin() const
{
    if (!content)
        return const_container_iterator();
    if (GetVariable())
        return const_container_iterator();
    const auto* container = content->GetIf<Container>();
    assert(container); // shouldn't be constructed or Id should be substituted already. What do ? 
    return container->begin();
}

const_container_iterator All::end() const
{
    if (!content)
        return const_container_iterator();
    if (GetVariable())
        return const_container_iterator();
    const auto* container = content->GetIf<Container>();
    assert(container); // shouldn't be constructed or Id should be substituted already. What do ? 
    return container->end();

}

std::ostream& operator<<(std::ostream& os, const All& all)
{
    os << all.ope;
    if (!all.empty())
        os << *all;
    return os;
}
}