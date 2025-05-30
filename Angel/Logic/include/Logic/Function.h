#pragma once
#include "Logic/Hypothesis.h"
#include <functional>
#include <string>

namespace Angel::Logic
{
class Knowledge;
class Expression;

class Function
{
public:
    using CallbackT = Expression(const Knowledge& k, Hypothesis& args);
    using Callback = std::function<CallbackT>;

    explicit Function(Callback cb, const std::string_view doc);

    Expression Matches(const Expression& expression, const Hypothesis& hypothesis) const;
    Expression Infer(const class Knowledge& k, Hypothesis& hypothesis) const;
    explicit operator bool() const;
	bool operator==(const Function& other) const;
    std::string_view Documentation() const;
    std::string_view ShortDescription() const;
    std::size_t Hash() const;
private:
    std::string documentation;
    Callback callback;
};

std::ostream& operator<<(std::ostream& os, const Function& id);

}