#include "Interpreter/Source.h"
#include <fstream>
#include <string>

namespace Interpreter 
{

Source::Source(const std::string_view data) :
    std::istringstream(std::string(data))
{
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
}

 
}