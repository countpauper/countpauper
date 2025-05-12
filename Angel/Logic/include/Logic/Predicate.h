#pragma once
#include "Logic/Id.h"
#include "Logic/List.h"
#include "Logic/Match.h"

namespace Angel::Logic
{


class Predicate
{
public:

//    Predicate(const Predicate& other);
//    Predicate(Predicate&& other);
    Predicate() : id("") {}
    explicit Predicate(const Id& id, List&& arguments={});
    explicit Predicate(const std::string& name , List&& arguments={});

    operator bool() const;
    bool operator==(const Predicate& other) const;
    std::size_t Hash() const;
    Match Matches(const Predicate& other, const Variables& substitutions) const;
    /*
    Object Copy() const override;
    const Object* Condition() const override;
    Object Infer(const Knowledge& known, const Variables& substitutions) const override;
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const override;
*/
private:
    friend std::ostream& operator<<(std::ostream& os, const Predicate& );
    Id id;
	List arguments;
};

std::ostream& operator<<(std::ostream& os, const Predicate& );

/*
Object predicate(const Id& id, Sequence&& arguments = Sequence());
Object predicate(const std::string& name, Sequence&& arguments = Sequence());

*/

}