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

    constexpr std::string_view whitespace = " \t\n\r\f\v";
    inline std::string_view rtrim(const std::string_view s, const std::string_view t = whitespace)
    {
        return s.substr(0,s.find_last_not_of(t)+1);
    }
    inline std::string_view ltrim(const std::string_view s, const std::string_view t = whitespace)
    {
        return s.substr(s.find_first_not_of(t));
    }
    inline std::string_view trim(const std::string_view s, const std::string_view t = whitespace)
    {
        return rtrim(ltrim(s, t),t);
    }
}