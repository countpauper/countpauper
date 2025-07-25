#include "Logic/Internal/Container.h"
#include "Logic/Expression.h"
#include "Logic/Internal/container_iterator.h"
#include <sstream>
#include <cassert>

namespace Angel::Logic
{

Container& Container::operator=(const Container& e)
{
    ContainerVariant::operator=(e);
    return *this;
}

std::size_t Container::size() const
{
    return std::visit(
        [this](const auto& obj)
        {
            return obj.size();
        }, *this);   
}

Expression Container::Simplify() const
{
    return std::visit(
        [this](const auto& obj) -> Expression
        {
            return obj.Simplify();
        }, *this);      
}

Set Container::Assumptions() const
{
    return std::visit(
        [this](const auto& obj)
        {
            return obj.Assumptions();
        }, *this);

}

Container Container::Substitute(const Hypothesis& hypothesis) const
{
    return std::visit(
        [&hypothesis](const auto& obj) -> Container
        {
            return Container(obj.Substitute(hypothesis));
        },   *this);      
}


Expression Container::Matches(const Expression& e, const Hypothesis& hypothesis) const 
{
    return std::visit(
        [&e, &hypothesis](const auto& obj) 
        {
            return obj.Matches(e, hypothesis);
        },   *this);    
}

Expression Container::Infer(const class Knowledge& knowledge, const Hypothesis& hypothesis, Trace& trace) const
{
    return std::visit(
        [&knowledge, &hypothesis, &trace](const auto& obj) -> Expression {
            return obj.Infer(knowledge, hypothesis, trace);
        }, *this);
}

Expression Container::GetItem(const Expression& key) const
{
    return std::visit(
        [&key](const auto& obj) {
            return obj.Get(key);
        }, *this);
}

Container::operator bool() const
{
    return size()!=0;
}

bool Container::operator==(const Container& rhs) const
{
    return std::visit(
        [this](const auto& rho)
        {
            return operator==(rho);
        }, rhs);
}


Container Container::operator+=(const Container& rhs)
{
    return std::visit(overloaded_visit{
        [&rhs](List& list) -> Container
        {
            return list += rhs;
        },
        [&rhs](Set& set) -> Container
        {
            return set += rhs;
        },
        [](All& all) -> Container
        {
            assert(false); // unimplemented
            return Container(List{});
        },
        [this](auto&) -> Container
        {
            throw std::invalid_argument(std::format("Can not add {} to container", AlternativeTypeId().name()));
        }
    }, *this);
}


Container Container::operator-=(const Container& rhs)
{
    return std::visit(overloaded_visit{
        [&rhs](List& list) -> Container
        {
            return list -= rhs;
        },
        [&rhs](Set& set) -> Container
        {
            return set -= rhs;
        },
        [this](auto&) -> Container
        {
            throw std::invalid_argument(std::format("Can not subtract {} to container", AlternativeTypeId().name()));
        }
    }, *this);
}

Container Container::operator&=(const Container& rhs)
{
    return std::visit(overloaded_visit{
        [&rhs](List& list) -> Container
        {
            return list &= rhs;
        },
        [&rhs](Set& set) -> Container
        {
            return set &= rhs;
        },
        [this](auto&) -> Container
        {
            throw std::invalid_argument(std::format("Can not intersect {} with container", AlternativeTypeId().name()));
        }
    }, *this);
}

Container Container::operator|=(const Container& rhs)
{
    return std::visit(overloaded_visit{
        [&rhs](List& list) -> Container
        {
            return list |= rhs;
        },
        [&rhs](Set& set) -> Container
        {
            return set |= rhs;
        },
        [this](auto&) -> Container
        {
            throw std::invalid_argument(std::format("Can not disjunct {} with container", AlternativeTypeId().name()));
        }
    }, *this);
}

Container Container::operator^=(const Container& rhs)
{
    return std::visit(overloaded_visit{
        [&rhs](List& list) -> Container
        {
            return list ^= rhs;
        },
        [&rhs](Set& set) -> Container
        {
            return set ^= rhs;
        },
        [this](auto&) -> Container
        {
            throw std::invalid_argument(std::format("Can not exclude {} with container", AlternativeTypeId().name()));
        }
    }, *this);
}

const_container_iterator Container::begin() const
{
    return std::visit(
         [](const auto& obj) 
         {
            return const_container_iterator{obj.begin()};
         }, *this);
}

const_container_iterator Container::end() const
{
    return std::visit(
         [](const auto& obj) 
         {
            return const_container_iterator{obj.end()};
         }, *this);
}


const_container_iterator Container::find(const Expression& key) const
{
    return std::find_if(begin(), end(), [&key](const Expression& item)
    { 
        if (item == key)
            return true;
        else if (const auto* association = item.GetIf<Association>())
            return association->Left() == key;
        else 
            return false;
    }); 
}

const_container_iterator Container::erase(const_container_iterator item)
{
    return std::visit(overloaded_visit{
        [&item](List& list) 
        {
            return const_container_iterator{list.erase(std::get<List::const_iterator>(item))};
        },
        [&item](Set& set) 
        {
            return const_container_iterator{set.erase(std::get<Set::const_iterator>(item))};
        },
        [&item](auto&)
        {
            assert(false); // unimplemented erase 
            return item;
        }
        }, *this);   
}
bool Container::operator<(const Container& o) const
{
    return Hash() < o.Hash();
}

std::string Container::Summary() const
{
    return std::visit(overloaded_visit{
         [this](const All& all) -> std::string 
         {
            if (all)
            {
                return std::format("All of {}", all->Summary());
            }
            else 
            {
                return "All";
            }
        },
        [](const auto& container) -> std::string 
        {
            return std::format("{} {} container with {} items", 
                container.unique ? "unique" : "non-unique",
                container.ordered ? "ordered" : "unordered", 
                container.size()); 
        }
    }, *this);
}

std::string to_string(const Container& c)
{
    std::stringstream ss;
    ss << c;
    return ss.str();
}

Container operator+(Container lhs, const Container& rhs)
{
    return lhs += rhs;
}

Container operator-(Container lhs, const Container& rhs)
{
    return lhs -= rhs;
}

Container operator&(Container lhs, const Container& rhs)
{
    return lhs &= rhs;
}

Container operator|(Container lhs, const Container& rhs)
{
    return lhs |= rhs;
}

Container operator^(Container lhs, const Container& rhs)
{
    return lhs ^= rhs;
}

std::ostream& operator<<(std::ostream& s, const Container& c)
{
    std::visit(
        [&s](const auto& obj)
        {
            s << obj;
        }, c);
    return s;
}



static_assert(std::input_iterator<const_container_iterator>);
    
}