#include "Interpreter/Source.h"
#include <fstream>
#include <string>
#include <cassert>
#include <iterator>

namespace Interpreter 
{


Source::Source(const std::string_view data) :
    buffer(data)
{
    if (data.size()<32)
        name = data;
    else 
        name = std::format("{} bytes of input", data.size());
}

Source::Source(const char* data) : 
    Source(std::string_view(data))
{
}

std::size_t Source::size() const
{
    return buffer.size();
}

SourceSpan Source::span(std::size_t from, std::size_t length) const
{
    length = std::min(length, size()-from);
    return SourceSpan{from, length, this};
}

std::string Source::Read(std::size_t from, std::size_t length) const
{
    if (from+length>buffer.size())
        throw std::out_of_range(std::format("Source {} buffer underrun: {} + {} > {}", name, from, length, buffer.size()));
    return buffer.substr(from, length);
}

std::string Source::Name() const 
{
    return name;
}

FileSource::FileSource(std::filesystem::path fn) :
    Source(std::string_view())
{
    std::ifstream file{fn};
    if ( !file )
        throw std::runtime_error(std::format("Source file `{}` can't be opened", fn.string()));
    buffer = std::string(std::istreambuf_iterator<char>{file}, {});
    name = fn.string();
}

 
}