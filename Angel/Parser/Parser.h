#pragma once
#include <string>
#include <map>
#include <memory>
#include <optional>
#include <any>

namespace Angel::Parser::BNF
{
    class Parser
    {
    public:
        virtual std::any Parse(const std::string_view rule, const std::any& tokens, const std::string_view value) const = 0;
        virtual std::any Merge(const std::any& left, const std::any& right) const = 0;
    };
}