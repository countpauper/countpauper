#include "File/Json.h"
#include <fstream>

namespace Engine
{

json LoadJson(std::string_view filename)
{
    std::ifstream file(std::string(filename).c_str());
    return json::parse(file);
}

void SaveJson(const json& data, std::string_view filename)
{
    std::ofstream file(std::string(filename).c_str());
    file << data;
}

template<>
std::optional<json> try_get<json>(const json& j, std::string_view key)
{
        auto it = j.find(key);
        if (it == j.end())
        {
                return std::optional<json>();
        }
         else
        {
                return it.value();
        }
}


}
