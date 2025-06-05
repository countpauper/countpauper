#include "Logic/Expression.h"
#include "Logic/Knowledge.h"

// File split so Expression.obj doesn't get too big due to templates
// Can be split further
namespace Angel::Logic
{

bool Expression::operator==(const Expression& rhs) const
{
    return std::visit(overloaded_visit{
        [this](std::monostate)
        {
            return !bool(*this);
        },
        [this](const auto& rho)
        {
            return operator==(rho);
        }
        }, rhs);
}


bool Expression::operator<(const Expression& rhs) const
{
    return std::visit(overloaded_visit{
        [this](std::monostate)
        {
            return !bool(*this);
        },
        [this](const auto& rho)
        {
            return (*this) < rho;
        }
        }, rhs);
}

}
