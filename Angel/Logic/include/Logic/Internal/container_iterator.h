#pragma once

#include "Logic/Set.h"
#include "Logic/List.h"
#include "Logic/All.h"
#include <variant>


namespace Angel::Logic
{


class const_container_iterator 
{
public:
    std::variant<std::monostate, List::const_iterator, Set::const_iterator> internal;
    Expression operator*() const;
    const_container_iterator& operator++();
    const_container_iterator operator++(int);
    operator bool() const;
    bool operator==(const const_container_iterator& other) const;
    bool operator!=(const const_container_iterator& other) const;
};

}
