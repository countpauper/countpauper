#pragma once
#include <string>
#include <map>
#include <memory>
#include <optional>
#include <any>

namespace Angel::Parser::BNF
{
    class Match
    {
    public:
        Match(const std::string_view r, const std::any& i= std::any()) :
            remaining(r),
            interpretation(i)
        {
        }
        const std::string_view remaining;
        std::any interpretation;
    };
    using PossibleMatch = std::optional<Match>;
}