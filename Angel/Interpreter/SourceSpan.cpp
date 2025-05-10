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

    // The Source is const and its position and flags are changed
    // but the contents itself is not and the position is restored. 
    // stream flags should not be restored as they indicate read failure status
    // NB: this might be misleading with multithreaded reading, as it is not atomic yet
    std::istream& stream = const_cast<std::istream&>(*static_cast<const std::istream*>(source));
    stream.clear();

    std::string result(length, '\x0');
    auto original = stream.tellg();
    stream.seekg(from);
    stream.read(result.data(), length);
    stream.seekg(original);
    result.erase(result.begin() + stream.gcount(), result.end());
    return result;
}

std::ostream& operator<<(std::ostream& os, const SourceSpan& span)
{
    os << std::string(span).c_str();
    return os;
}



}