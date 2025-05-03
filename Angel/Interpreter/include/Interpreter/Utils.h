#pragma once 

// helper function for std::visit
template<class... Ts> struct overloaded_visit : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
//template<class... Ts> overloaded_visit(Ts...) -> overloaded_visit<Ts...>;
