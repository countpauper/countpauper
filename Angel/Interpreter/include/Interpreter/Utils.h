#pragma once 
#include <string>

// helper function for std::visit
template<class... Ts> struct overloaded_visit : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
//template<class... Ts> overloaded_visit(Ts...) -> overloaded_visit<Ts...>;

namespace Interpreter
{
    std::string Unclose(const std::string_view in, char left, char right);
    std::string Unclose(const std::string_view in, char c);
}