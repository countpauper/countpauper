#pragma once 

#include <cstddef>
#include <iostream>

namespace Interpreter 
{
    struct SourceSpan {
        size_t from = 0;
        size_t length = 0;

        size_t size() const;
        bool empty() const;
        operator std::string() const;
        bool operator==(const SourceSpan& o) const;
        bool operator!=(const SourceSpan& o) const { return !(*this == o); }
        SourceSpan sub(std::ptrdiff_t offset, std::ptrdiff_t newLength) const;
        std::string extract(std::istream& s) const;
        class iterator
        {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = std::size_t;
    
            explicit iterator(value_type i) ;
            value_type operator*() const;
            iterator& operator++();
            iterator operator++(int) ;
            bool operator==(const iterator& other) const;
            bool operator!=(const iterator& other) const { return !(*this == other); }
        private:
            value_type idx;
        };
    
        iterator begin() const;
        iterator end() const;
        
    };    

    std::ostream& operator<<(std::ostream& os, const SourceSpan& span);
}