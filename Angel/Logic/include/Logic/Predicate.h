#pragma once
#include "Logic/Id.h"
#include "Logic/List.h"
namespace Angel::Logic
{


class Predicate
{
public:

//    Predicate(const Predicate& other);
//    Predicate(const Id& id, Sequence&& arguments = Sequence());
//    Predicate(Predicate&& other);
    explicit Predicate(const std::string& name , List&& arguments={});

    operator bool() const;
    bool operator==(const Predicate& other) const;
    std::size_t Hash() const;
    /*
    Object Copy() const override;
    Match Matching(const Expression& other, const Variables& substitutions) const override;
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