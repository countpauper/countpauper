#include "stdafx.h"
#include "Sequence.h"
#include "Boolean.h"
#include "Set.h"

namespace Angel::Logic
{

Sequence::Sequence()
{
}

Sequence::Sequence(Object&& value)
{
    if (value)
        emplace_back(value);
}

Sequence::Sequence(Sequence&& other) 
{
    reserve(other.size());
    for (auto& o : other)
        emplace_back(std::move(o));
}

Sequence::Sequence(const Sequence& other)
{
    for (const auto& io : other)
    {
        Add(io->Copy());
    }
}

bool Sequence::operator==(const Expression& other) const
{
    if (auto seq = dynamic_cast<const Sequence*>(&other))
    {
        if (size() != seq->size())
            return false;
        auto it = seq->begin();
        for (const auto& e : *this)
        {
            if (*it++ != e)
                return false;
        }
        return true;
    }
    return false;
}

std::string Sequence::String() const
{
    std::string result;
    for (const auto& element: *this)
    {
        if (!result.empty())
            result += ",";
        result += element->String();
    }
    return "("+result+")";
}


Object Sequence::Copy() const
{
    return Create<Sequence>(*this);
}

Object Sequence::Match(const Expression& expr) const
{
    if (auto seq = dynamic_cast<const Sequence*>(&expr))
    {
        if (size() != seq->size())
            return boolean(false);
        Sequence matchedSequence;
        auto it = seq->begin();
        for (const auto& e : *this)
        {
            auto elementMatch = e.Match(**it);
            // TODO: return a conjunction of the results if they are not elemental? Variables? 
            // non lazy? still lazy for trivial? 
            if (!elementMatch.Trivial())
                return boolean(false);
            ++it;	// TODO: zip
        }
        return boolean(true);
    }
    // TODO: cast array to sequence then match?
    return boolean(false);
}

Object Sequence::Cast(const std::type_info& t, const Knowledge& k) const
{
    if (t == typeid(Boolean))
    {
        return boolean(!empty());
    }
    else if (t == typeid(Set))
    {
        auto result = Create<Set>();
        for (const auto& e : *this)
        {
            result.As<Set>()->Add(Object(e));
        }
        return result;
    }
    throw CastException<Sequence>(t);
}

void Sequence::Add(Object&& value)
{
    if (value)
    {
        emplace_back(std::move(value));
    }
}

void Sequence::Merge(Sequence&& other)
{
    for (auto& o : other)
        emplace_back(std::move(o));
}

Object sequence()
{
    return Create<Sequence>();
}

Object sequence(Sequence&& seq)
{
    return Create<Sequence>(seq);
}

Object sequence(Sequence&& left, Sequence&& right)
{
    auto result = sequence(left);
    result.As<Sequence>()->Merge(std::move(right));
    return result;
}

Object sequence(Sequence&& left, Object&& right)
{
    auto result = sequence(left);
    result.As<Sequence>()->Add(std::move(right));
    return result;
}

Object sequence(Object&& left, Sequence&& right)
{
    auto result = sequence(Sequence(std::move(left)));
    result.As<Sequence>()->Merge(std::move(right));
    return result;
}

}