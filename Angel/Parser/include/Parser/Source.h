#pragma once 

#include <sstream>
#include <filesystem>

namespace Parser 
{
class SourceSpan;

class Source 
{
public:
    Source(const std::string_view data="");
    Source(const char* data);
    std::size_t size() const;
    SourceSpan span(std::size_t from=0, std::size_t length=-1) const;
    using const_iterator = std::string::const_iterator;
    const_iterator begin() const;
    const_iterator end() const;
    
    std::string Read(std::size_t from, std::size_t length) const;
    char Read(std::size_t from) const;
    std::string Name() const;
protected:
    std::string buffer;
    std::string name;
};

class FileSource : public Source
{
public:
    FileSource(std::filesystem::path file);
};

}