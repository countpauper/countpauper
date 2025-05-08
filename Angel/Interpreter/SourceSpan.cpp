#include "Interpreter/SourceSpan.h"
#include "Interpreter/Source.h"
#include <string>
#include <algorithm>

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
    return from == o.from && length == o.length; 
}

SourceSpan SourceSpan::sub(std::ptrdiff_t offset, std::ptrdiff_t newLength) const 
{ 
    std::ptrdiff_t newStart = std::max(0LL, offset + static_cast<std::ptrdiff_t>(from));
    std::ptrdiff_t maxLength = (from+length - newStart);
    if (newLength<0)
        newLength = std::max(0LL, static_cast<std::ptrdiff_t>(length) + newLength);
    return SourceSpan(newStart, std::min(newLength, maxLength));
}


std::string SourceSpan::extract(Source& stream) const
{
    std::string result(length, '\x0');
    stream.seekg(from);
    stream.read(result.data(), length);
    result.erase(result.begin() + stream.gcount(), result.end());
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