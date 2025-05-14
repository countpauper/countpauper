#pragma once
#include "Logic/Boolean.h"
#include "Logic/Integer.h"
#include "Logic/Id.h"
#include "Logic/Variable.h"
#include "Logic/VariantUtils.h"
#include <variant>

namespace Angel::Logic
{

using ElementVariant = std::variant<Boolean, Integer, Id>;

template < typename T >
concept IsElement = is_alternative<T, ElementVariant>;

class Element : public ElementVariant
{
public:
    template<typename T>
    requires is_alternative<T, ElementVariant>
    Element(const T& v) :
        ElementVariant(v)
    {
    }
    explicit operator bool() const;
};

}
