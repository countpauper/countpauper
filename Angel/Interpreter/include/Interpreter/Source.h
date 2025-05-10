#pragma once 
#include "Interpreter/SourceSpan.h"

#include <sstream>
#include <filesystem>

namespace Interpreter 
{

class Source 
{
public:
    Source(const std::string_view data="");
    Source(const char* data);
    std::size_t size() const;
    SourceSpan span(std::size_t from=0, std::size_t length=-1) const;
    
    std::string Read(std::size_t from=0, std::size_t length=-1) const;
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