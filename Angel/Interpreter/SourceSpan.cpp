#include "Interpreter/SourceSpan.h"
#include "Interpreter/Source.h"
#include <string>
#include <algorithm>
#include <cassert>

namespace Interpreter 
{

size_t SourceSpan::offset() const
{
    return from;
}

size_t SourceSpan::size() const 
{ 
    return length; 
}

bool SourceSpan::empty() const 
{ 
    return size()==0; 
}

SourceSpan::const_iterator SourceSpan::begin() const
{
    return const_iterator(from, source);
}

SourceSpan::const_iterator SourceSpan::end() const
{
    return const_iterator(from + length, source);

}
SourceSpan::operator std::string() const
{
    if (source)
        return std::format("{}[{}:{}]", source->Name(), from, length);
    else
        return std::format("[{}:{}]", from, length);
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
        newLength = std::max(0LL, static_cast<std::ptrdiff_t>(length) - offset + newLength);
    newLength = std::min(newLength, maxLength);
    assert(!source || newStart + newLength <= source->size());
    return SourceSpan{static_cast<std::size_t>(newStart), static_cast<std::size_t>(newLength), source};
}


std::string SourceSpan::extract() const
{
    if (!source)
        throw std::runtime_error(std::format("Can't extract {} bytes from null source", length));
    return std::string(begin(), end());
}

std::ostream& operator<<(std::ostream& os, const SourceSpan& span)
{
    os << std::string(span).c_str();
    return os;
}

SourceSpan::const_iterator::const_iterator(std::size_t pos, const Source* source) : 
    pos(pos), 
    source(source) 
{
}

SourceSpan::const_iterator& SourceSpan::const_iterator::operator++() 
{ 
    pos++; 
    return *this; 
}
SourceSpan::const_iterator& SourceSpan::const_iterator::operator+=(std::ptrdiff_t offset)
{
    pos+=offset;
    return *this;
}

SourceSpan::const_iterator SourceSpan::const_iterator::operator++(int) 
{ 
    auto tmp = *this; 
    ++pos; 
    return tmp;  
}

SourceSpan::const_iterator& SourceSpan::const_iterator::operator--() 
{ 
    pos--; 
    return *this; 
}
SourceSpan::const_iterator& SourceSpan::const_iterator::operator-=(std::ptrdiff_t offset)
{
    pos-=offset;
    return *this;
}

SourceSpan::const_iterator SourceSpan::const_iterator::operator--(int) 
{ 
    auto tmp = *this; 
    --pos; 
    return tmp;  
}

SourceSpan::const_iterator::value_type SourceSpan::const_iterator::operator*() const 
{ 
    return source->Read(pos);
}

bool SourceSpan::const_iterator::operator==(const SourceSpan::const_iterator& o) const 
{ 
    return pos == o.pos && source == o.source;
}

SourceSpan::const_iterator operator+(const SourceSpan::const_iterator& it, std::ptrdiff_t offset)
{
    SourceSpan::const_iterator result = it;
    return result+=offset;
}

std::ptrdiff_t operator-(const SourceSpan::const_iterator& lhs, const SourceSpan::const_iterator& rhs)
{
    return lhs.pos - rhs.pos;
}

}