#pragma once 

#include <cstddef>
#include <iostream>

namespace Interpreter 
{
    class Source;

    struct SourceSpan 
    {
        size_t from = 0;
        size_t length = 0;
        const Source* source = nullptr;

        size_t offset() const;
        size_t size() const;
        bool empty() const;
        class const_iterator 
        {
        public:
            const_iterator() : pos(0), source(nullptr) {}
            using difference_type = std::ptrdiff_t;
            using value_type = char;
            const_iterator& operator++();
            const_iterator& operator+=(std::ptrdiff_t offset);
            const_iterator operator++(int);
            const_iterator& operator--();
            const_iterator& operator-=(std::ptrdiff_t offset);
            const_iterator operator--(int);
            value_type operator*() const;
            bool operator==(const const_iterator& o) const;
        protected:
            friend SourceSpan;
            friend std::ptrdiff_t operator-(const SourceSpan::const_iterator& lhs, const SourceSpan::const_iterator& rhs);

            const_iterator(std::size_t pos, const Source* source);
            size_t pos;
            const Source* source; 
        };
        const_iterator begin() const;
        const_iterator end() const;         
        operator std::string() const;
        bool operator==(const SourceSpan& o) const;
        bool operator!=(const SourceSpan& o) const { return !(*this == o); }
        SourceSpan sub(std::ptrdiff_t offset, std::ptrdiff_t newLength=std::numeric_limits<std::ptrdiff_t>::max()) const;
        std::string extract() const;
        
    };    
    SourceSpan::const_iterator operator+(const SourceSpan::const_iterator& it, std::ptrdiff_t offset);
    std::ptrdiff_t operator-(const SourceSpan::const_iterator& lhs, const SourceSpan::const_iterator& rhs);
    
    std::ostream& operator<<(std::ostream& os, const SourceSpan& span);
}