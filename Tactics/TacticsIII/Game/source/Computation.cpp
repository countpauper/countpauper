#include "Game/Computation.h"
#include <numeric>
#include <sstream>
#include <cassert>

namespace Game
{


Computation::Computation(int constant) :
    Computation(constant, "")
{
}

Computation::Computation(int value, std::string_view description) :
    operations{
        {std::string(description), Operator::add, value}
    }
{
}

Computation::Computation(Engine::Range<int> limit) :
    limit(limit)
{
}

int Operation::operator()(int input) const
{
    switch(op)
    {
        case Operator::add:
            return input + value;
        case Operator::multiply:
            return input * value;
        case Operator::lower_bound:
            return std::max(input, value);
        case Operator::upper_bound:
            return std::min(input, value);
        default:
            throw std::runtime_error("Unimplemented operator "+std::to_string(unsigned(op)));
    }
}

bool Operation::operator==(const Operation& o) const
{
    return op == o.op &&  value == o.value && description == o.description;
}

bool Operation::Redundant() const
{
    switch(op)
    {
        case Operator::add:
            return value == 0;
        case Operator::multiply:
            return value == 1;
        case Operator::lower_bound:
            return value == std::numeric_limits<int>::min();
        case Operator::upper_bound:
            return value == std::numeric_limits<int>::max();
        default:
            throw std::runtime_error("Unimplemented redundancy "+std::to_string(unsigned(op)));

    }
}

std::ostream& operator<<(std::ostream& s, const Operation& operation)
{
    switch(operation.op)
    {
        case Operator::add:
            if (s.tellp())
            {
                if (operation.value >= 0 )
                    s << " + " << operation.value;
                else
                    s << " - " << -operation.value;
            }
            else
            {
                s << operation.value;
            }
            break;
        case Operator::multiply:
            s << " * " << operation.value;
            break;
        case Operator::lower_bound:
            s << " >= " << operation.value;
            break;
        default:
            throw std::runtime_error("Unimplemented stream for operator "+std::to_string(unsigned(operation.op)));
    }
    s << "[" << operation.description << "]";
    return s;
}

int Computation::Total() const
{
    int result = 0;
    for(const auto& op: operations)
    {
        result = op(result);
    }
    return limit.Clip(result);
}

std::string Computation::Description() const
{
    if (IsConstant())
    {
        return std::to_string(Total());
    }

    std::stringstream stream;

    for(const auto& op: operations)
    {
        stream << op;
    }
    return stream.str();
}


bool Computation::empty() const
{
    return operations.empty();
}

Operation Computation::AsValue(Operator op) const
{
    if (empty())
    {
        assert(false); // should avoid this. It's not clear what this means
        return Operation{"", op, 0};
    }
    else if ((operations.size() == 1) && (operations.back().op == Operator::add))
    {   // basically a single constant
        const auto& value = operations.front();
        return Operation{value.description, op, value.value};
    }
    else
    {   // nested computation, compute the result now
        return Operation{Description(), op, Total()};
    }

}

Computation& Computation::operator+=(const Computation& o)
{
    operations.insert(operations.end(), o.operations.begin(), o.operations.end());
    return *this;
}


Computation& Computation::operator-=(const Computation& o)
{
    for(const auto& o: o.operations)
    {
        operations.emplace_back(o.description, o.op, -o.value);
    }
    return *this;
}

Computation& Computation::operator*=(const Computation& o)
{
    if (o.empty())
    {   // A * 0 = 0
        operations.clear();
        return *this;
    }
    return append(Operator::multiply, o);
}

bool Computation::operator==(const Computation& o) const
{
    return limit == o.limit &&
        operations == o.operations;
}

Computation& Computation::Simplify()
{
    std::erase_if(operations, [](const Operation& o)
    {
        return o.Redundant();
    });
    return *this;
}

bool Computation::IsConstant() const
{
    return operations.size() == 1 && operations.front().op == Operator::add && operations.front().description.empty();
}

Computation& Computation::append(Operator op, const Computation& o)
{
    operations.push_back(o.AsValue(op));
    return *this;
}

Computation operator+(const Computation& a, const Computation& b)
{
    Computation result(a);
    return result+=b;
}

Computation operator-(const Computation& a, const Computation& b)
{
    Computation result(a);
    return result -= b;
}

Computation operator*(const Computation& a, const Computation& b)
{
    Computation result(a);
    return result *= b;
}

Computation operator>=(const Computation& a, const Computation& b)
{
    if (a.Total()>= b.Total())
    {
        return a;
    }
    else
    {
        return b;
    }
}

}
