#pragma once

#include "Logic/Node.h"
#include "Logic/Element.h"
#include "Logic/Predicate.h"
#include "Logic/Set.h"
#include "Logic/List.h"
#include "Logic/CastException.h"
#include <variant>
#include <vector>
#include <map>
#include <type_traits>

namespace Angel::Logic
{

// Use with std::visit to provide a set of explicitly typed alternatives
// or variants with a subset of alternatives
template<class... Ts> struct overloaded_visit : Ts... { using Ts::operator()...; };
template<class T, typename Var>

// is_alternative<T, Variant> is true if T is one of the variants
concept is_alternative = requires (const Var& var) { 
  []<typename... Args>
    requires (std::same_as<T, Args> || ...) 
      (const std::variant<Args...>&){ } (var);
};

}
