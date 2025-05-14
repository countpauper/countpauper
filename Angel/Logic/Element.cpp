#include "Logic/Element.h"
#include "Logic/Knowledge.h"
#include <assert.h>

namespace Angel::Logic
{

Element::operator bool() const
{
    return std::visit(overloaded_visit{
        [](std::monostate) {
            return false;
        },
        [](const auto& e) {
            return bool(e);
        }
    }, *this);
}
}

