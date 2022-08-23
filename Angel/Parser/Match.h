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
        using Rules = std::vector<const struct Rule*>;
        Match(const std::string_view r, const std::any& t= std::any(), const Rules& rules= Rules()) :
            remaining(r),
            tokens(t),
            rules(rules)
        {
        }
        const std::string_view remaining;
        std::any tokens;
        Rules rules;
    };
    using PossibleMatch = std::optional<Match>;
}