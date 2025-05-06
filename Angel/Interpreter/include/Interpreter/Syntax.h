#pragma once 

#include "Rule.h"
#include <string>
#include <map>
#include <ranges>

namespace Interpreter 
{

    class Syntax 
    {
    public:

        explicit Syntax(std::initializer_list<Rule> rules={}, const std::string_view start="");
        Syntax(Syntax&& other);

        using LookupTable = std::multimap<Symbol, Rule>;
        class const_iterator 
        {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = Rule;
            const_iterator(LookupTable::const_iterator it) : wrapped(it) {}
            const_iterator& operator++() { wrapped++; return *this; }
            const_iterator operator++(int) { auto tmp = *this; ++wrapped; return tmp;  }
            const value_type& operator*() const { return wrapped->second; }
            const value_type* operator->() const { return &wrapped->second; }
            bool operator==(const const_iterator& o) const { return wrapped == o.wrapped; }
        protected:
            LookupTable::const_iterator wrapped;
        };
        class iterator : public const_iterator
        {
        public:
            iterator(LookupTable::iterator it) : const_iterator(it) {}
            iterator& operator++() { wrapped++; return *this; }
            iterator operator++(int) { auto tmp = *this; ++wrapped; return tmp;  }
            value_type& operator*() { return reinterpret_cast<LookupTable::iterator&>(wrapped)->second; }
            value_type* operator->() { return &reinterpret_cast<LookupTable::iterator&>(wrapped)->second; }
        };
        bool empty() const;
        std::size_t size() const;
        const_iterator begin() const;
        const_iterator end() const;
        iterator begin();
        iterator end();
        std::ranges::subrange<LookupTable::const_iterator> Lookup(Symbol symbol) const;
        std::ranges::subrange<LookupTable::const_iterator> operator[](Symbol symbol) const;
        Symbol Root() const;
        template< class... Args >
        iterator emplace(Args&&... args )
        {
            Rule rule(std::forward<Args>(args)...);
            auto it = lookup.emplace(rule.symbol, std::move(rule));
            if (!root)
                root = it->first;
            return iterator(it);
        }
        bool IsLeftRecursive() const;
    private:
        using SyntaxPath = std::deque<Symbol>;
        bool CheckLeftRecursive(SyntaxPath& symbol) const;
        void CreateLookup();
        LookupTable lookup;
        Symbol root;
    };

    static_assert(std::input_iterator<Syntax::const_iterator>);
}