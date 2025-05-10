#include "Interpreter/SourceSpan.h"
#include "Interpreter/Source.h"
#include <string>
#include <algorithm>
#include <cassert>

namespace Interpreter 
{

size_t SourceSpan::size() const 
{ 
    return length; 
}

bool SourceSpan::empty() const 
{ 
    return size()==0; 
}

SourceSpan::operator std::string() const
{
    return std::format("{}:{}", from, length);
}

bool SourceSpan::operator==(const SourceSpan& o) const 
{ 
    return from == o.from && length == o.length && source == o.source;
}

SourceSpan SourceSpan::sub(std::ptrdiff_t offset, std::ptrdiff_t newLength) const 
{ 
    std::ptrdiff_t newStart = std::max(0LL, offset + static_cast<std::ptrdiff_t>(from));
    std::ptrdiff_t maxLength = (from+length - newStart);
    if (newLength<0)
        newLength = std::max(0LL, static_cast<std::ptrdiff_t>(length) + newLength);
    newLength = std::min(newLength, maxLength);
    assert(!source || newStart + newLength <= source->size());
    return SourceSpan{static_cast<std::size_t>(newStart), static_cast<std::size_t>(newLength), source};
}


std::string SourceSpan::extract() const
{
    if (length==0)
        return std::string();
    if (!source)
        throw std::runtime_error(std::format("Can't extract {} bytes from null source", length));

    source->clear();

    std::string result(length, '\x0');
    source->seekg(from);
    source->read(result.data(), length);
    result.erase(result.begin() + source->gcount(), result.end());
    return result;
}

SourceSpan::iterator SourceSpan::begin() const
{
    return iterator(from);
}

SourceSpan::iterator SourceSpan::end() const
{
    return iterator(from+length);
}


SourceSpan::iterator::iterator(value_type i) :
    idx(i) 
{
}

SourceSpan::iterator::value_type SourceSpan::iterator::operator*() const
{
    return idx;
}

SourceSpan::iterator& SourceSpan::iterator::operator++()
{
    ++idx;
    return *this;
}

SourceSpan::iterator SourceSpan::iterator::operator++(int) 
{ 
    auto tmp(*this);
    ++*this;  
    return tmp; 
}
bool SourceSpan::iterator::operator==(const iterator& other) const 
{ 
    return (idx == other.idx); 
}


std::ostream& operator<<(std::ostream& os, const SourceSpan& span)
{
    os << std::string(span).c_str();
    return os;
}



}