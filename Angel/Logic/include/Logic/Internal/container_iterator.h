#pragma once

#include "Logic/Set.h"
#include "Logic/List.h"
#include "Logic/All.h"
#include <variant>


namespace Angel::Logic
{


class const_container_iterator : public std::variant<std::monostate, List::const_iterator, Set::const_iterator>
{
public:
    using difference_type = std::ptrdiff_t;
    using value_type = Expression;

    Expression operator*() const;
    const_container_iterator& operator++();
    const_container_iterator operator++(int);
    operator bool() const;
};

}
