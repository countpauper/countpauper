#pragma once 
#include "Interpreter/SourceSpan.h"

#include <sstream>
#include <filesystem>

namespace Interpreter 
{

class Source : public std::istringstream
{
public:
    Source(const std::string_view data="");
    SourceSpan span(std::size_t from=0, std::size_t length=-1);
    std::size_t size() const;
protected:
    std::string name;
};

class FileSource : public Source
{
public:
    FileSource(std::filesystem::path file);
};

}