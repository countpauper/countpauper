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
        using LookupTable = std::multimap<hash_t, Rule>;
        
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
        std::ranges::subrange<LookupTable::const_iterator> Lookup(hash_t symbol) const;
        std::ranges::subrange<LookupTable::const_iterator> operator[](hash_t symbol) const;
        hash_t Root() const;

        bool IsLeftRecursive() const;
        bool IsAmbiguous(size_t tokenDepth) const;
    private:
        using SyntaxPath = std::deque<hash_t>;
        bool CheckLeftRecursive(SyntaxPath& symbol) const;
        void CreateLookup();
        LookupTable lookup;
        hash_t root;
    };

    static_assert(std::input_iterator<Syntax::iterator>);
}