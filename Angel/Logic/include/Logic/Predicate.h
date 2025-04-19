#pragma once
#include "Object.h"
#include "Id.h"
#include "Sequence.h"

namespace Angel
{
namespace Logic
{

class Predicate : public Expression
{
public:
    Predicate(const Predicate& other);
    Predicate(const Id& id, Sequence&& arguments = Sequence());
    Predicate(const std::string& name, Sequence&& arguments=Sequence());
    Predicate(Predicate&& other);
    operator bool() const override;
	bool operator==(const Expression& other) const override;
    std::size_t Hash() const override;
    Object Copy() const override;
    Object Match(const Expression& other) const override;
    Object Infer(const Knowledge& known) const override;
protected:
    Object Cast(const std::type_info& t, const Knowledge& k) const override;
private:
    friend std::ostream& operator<<(std::ostream& os, const Predicate& );
    Id id;
	Sequence arguments;
};

std::ostream& operator<<(std::ostream& os, const Predicate& );

Object predicate(const Id& id, Sequence&& arguments = Sequence());
Object predicate(const std::string& name, Sequence&& arguments = Sequence());

}
}