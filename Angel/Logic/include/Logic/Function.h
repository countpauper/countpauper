#pragma once
#include <functional>
#include "Logic/Match.h"

namespace Angel::Logic
{
class Object;
class Knowledge;

class Function
{
public:
    using CallbackT = Object(const Knowledge& k, const Variables& vars);
    using Callback = std::function<CallbackT>;

    explicit Function(Callback cb, const std::string_view doc);

    Match Matches(const Expression& expression, const Variables& vars) const;
    Object Infer(const class Knowledge& k, const Variables& substitutions) const;
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