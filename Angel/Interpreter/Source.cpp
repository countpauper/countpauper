#include "Interpreter/Source.h"
#include <fstream>
#include <string>
#include <cassert>

namespace Interpreter 
{


Source::Source(const std::string_view data) :
    std::istringstream(std::string(data))
{
    exceptions(badbit|failbit|eofbit);

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
    return this->rdbuf()->view().size();
}

SourceSpan Source::span(std::size_t from, std::size_t length) const
{
    length = std::min(length, size()-from);
    return SourceSpan{from, length, this};
}

FileSource::FileSource(std::filesystem::path fn) :
    Source(std::string_view())
{
    std::ifstream file{fn};
    if ( !file )
        throw std::runtime_error(std::format("Source file `{}` can't be opened", fn.string()));
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    str(buffer.str());
    name = fn.string();
}

 
}