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
        class iterator 
        {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = Rule;
            iterator(LookupTable::const_iterator it) : wrapped(it) {}
            iterator& operator++() { wrapped++; return *this; }
            iterator operator++(int) { auto tmp = *this; ++wrapped; return tmp;  }
            const value_type& operator*() const { return wrapped->second; }
            bool operator==(const iterator& o) const { return wrapped == o.wrapped; }
        private:
            LookupTable::const_iterator wrapped;
        };
        bool empty() const;
        iterator begin() const;
        iterator end() const;
        std::ranges::subrange<LookupTable::const_iterator> Lookup(Symbol symbol) const;
        std::ranges::subrange<LookupTable::const_iterator> operator[](Symbol symbol) const;
        Symbol Root() const;
        template< class... Args >
        void emplace(Args&&... args )
        {
            Rule rule(std::forward<Args>(args)...);
            lookup.emplace(rule.symbol, std::move(rule));
        }
        bool IsLeftRecursive() const;
    private:
        using SyntaxPath = std::deque<Symbol>;
        bool CheckLeftRecursive(SyntaxPath& symbol) const;
        void CreateLookup();
        LookupTable lookup;
        Symbol root;
    };

    static_assert(std::input_iterator<Syntax::iterator>);
}