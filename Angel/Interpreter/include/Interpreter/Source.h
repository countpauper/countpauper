#pragma once 
#include <sstream>
#include <filesystem>

namespace Interpreter 
{

class Source : public std::istringstream
{
public:
    Source(const std::string_view data="");
};

class FileSource : public Source
{
public:
    FileSource(std::filesystem::path file);
};

}