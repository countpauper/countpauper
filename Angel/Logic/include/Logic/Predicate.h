#pragma once
#include "Logic/Id.h"
#include "Logic/List.h"
#include "Logic/Match.h"

namespace Angel::Logic
{


class Predicate
{
public:
    Predicate() : id("") {}
    explicit Predicate(const Id& id, List&& arguments={});
    explicit Predicate(const std::string& name , List&& arguments={});

    operator bool() const;
    bool operator==(const Predicate& other) const;
    std::size_t Hash() const;
    Match Matches(const Predicate& other, const Variables& substitutions) const;

private:
    friend std::ostream& operator<<(std::ostream& os, const Predicate& );
    Id id;
	List arguments;
};

std::ostream& operator<<(std::ostream& os, const Predicate& );


}