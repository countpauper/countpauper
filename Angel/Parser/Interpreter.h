#pragma once
#include <string>
#include <map>
#include <memory>
#include <optional>
#include <any>

namespace Angel::Parser::BNF
{
    class Interpreter
    {
    public:
        virtual std::any Interpret(const std::string_view rule, const std::any& interpretation, const std::string_view value) const = 0;
        virtual std::any Merge(const std::any& left, const std::any& right) const = 0;
    };
}